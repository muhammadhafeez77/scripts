// setup.c - dotfiles bootstrap for NixOS
// build: gcc -Wall -Wextra -O2 -o setup setup.c
// run:   sudo ./setup --execute

#include <limits.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

#define GH "https://github.com/muhammadhafeez77/"

static char user[256];
static char home[PATH_MAX];

// ---------------------------------------------------------------- helpers

static void die(const char *fmt, ...)
    __attribute__((noreturn, format(printf, 1, 2)));
static void die(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, RED "error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, RESET "\n");
  va_end(ap);
  exit(EXIT_FAILURE);
}

// printf-style system(); aborts on ANY failure so we never keep
// going (and keep running rm/mv) after something went wrong.
static void run(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static void run(const char *fmt, ...)
{
  char cmd[4096];
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(cmd, sizeof cmd, fmt, ap);
  va_end(ap);
  if (n < 0 || (size_t)n >= sizeof cmd)
    die("command too long");

  printf(YELLOW "+ %s\n" RESET, cmd);
  int rc = system(cmd);
  if (rc != 0)
    die("command failed (status %d): %s", rc, cmd);
}

// real chdir(): changes THIS process's directory (the old cd() only
// changed the directory of a throwaway child shell).
static void cd(const char *path)
{
  if (chdir(path) != 0)
  {
    perror(path);
    exit(EXIT_FAILURE);
  }
}

static int exists(const char *path)
{
  struct stat st;
  return lstat(path, &st) == 0; // lstat: a dangling symlink still "exists"
}

// Rename path -> path.bak.<timestamp> if it exists. Safe to re-run.
static void backup(const char *path)
{
  if (!exists(path))
    return;
  char dst[PATH_MAX + 32];
  snprintf(dst, sizeof dst, "%s.bak.%ld", path, (long)time(NULL));
  printf(YELLOW "+ backup %s -> %s\n" RESET, path, dst);
  if (rename(path, dst) != 0)
  {
    perror(path);
    exit(EXIT_FAILURE);
  }
}

// Shallow-clone a repo and strip the git metadata.
static void clone(const char *url, const char *dest, int keep_readme)
{
  run("git clone --depth 1 '%s' '%s'", url, dest);
  run("rm -rf '%s/.git' '%s/.gitignore'", dest, dest);
  if (!keep_readme)
    run("rm -f '%s/README.md'", dest);
}

static int confirm(const char *question)
{
  char answer = 0;
  printf("%s [y/N]: ", question);
  fflush(stdout);
  // fail CLOSED: EOF, garbage or anything but y/Y means "no"
  if (scanf(" %c", &answer) != 1)
    return 0;
  return answer == 'y' || answer == 'Y';
}

// ------------------------------------------------------------------- main
int main(int argc, char *argv[])
{
  if (argc < 2 || strcmp(argv[1], "--execute") != 0)
  {
    fprintf(stderr,
            RED "usage: sudo %s --execute [username]\nrequires: git\n" RESET,
            argv[0]);
    return EXIT_FAILURE;
  }

  if (geteuid() != 0)
    die("must be run as root (use sudo, or a root shell)");

  // Which user are we setting up? Priority:
  //   1. explicit argument:  ./setup --execute coffee   (works from a root
  //   shell)
  //   2. SUDO_USER:          sudo ./setup --execute     (set automatically by
  //   sudo)
  const char *target = NULL;
  if (argc >= 3 && *argv[2])
    target = argv[2];
  else if (getenv("SUDO_USER") && *getenv("SUDO_USER"))
    target = getenv("SUDO_USER");

  if (!target)
    die("can't tell which user to set up (you're probably in a root shell).\n"
        "  pass it explicitly:  %s --execute <username>",
        argv[0]);
  if (strcmp(target, "root") == 0)
    die("target must be a normal user, not root "
        "(root's files are handled automatically)");

  struct passwd *pw = getpwnam(target);
  if (!pw)
    die("no such user: %s", target);

  snprintf(user, sizeof user, "%s", pw->pw_name);
  snprintf(home, sizeof home, "%s", pw->pw_dir);

  printf("User: %s   Home: %s\n", user, home);
  if (!confirm("Are you sure you want to continue?"))
  {
    fprintf(stderr, RED "aborted.\n" RESET);
    return EXIT_FAILURE;
  }
  printf(GREEN "\n==== Begin ====\n" RESET);

  cd(home);
  run("mkdir -p '%s/Downloads' '%s/Documents' '%s/.config' "
      "'%s/.local/bin' '%s/.local/share' '%s/.local/share/fonts' "
      "'%s/.themes' '%s/.icons' '%s/.zsh-theme' /root/.config",
      home, home, home, home, home, home, home, home, home);

  // ---- /etc/nixos ------------------------------------------------------
  // Keep .git here if you want to `git pull` your flake later:
  // change the last argument of clone() to see README, and skip the
  // rm of .git if you prefer. Here we mirror the original behaviour.
  char tmp[PATH_MAX + 32];
  snprintf(tmp, sizeof tmp, "%s/nixos-dotfiles", home);
  backup(tmp);
  clone(GH "Nixos-Flakes.git", tmp, 0);

  run("cp /etc/nixos/hardware-configuration.nix "
      "'%s/hardware-configuration.nix'",
      tmp);
  backup("/etc/nixos"); // timestamped, so re-running is safe
  run("mv '%s' /etc/nixos", tmp);
  run("ln -sfn /etc/nixos '%s/nixos-dotfiles'", home);

  // ---- user configs ----------------------------------------------------
  printf("\n==== INSTALLING CONFIGS ====\n\n");
  cd(home);

  backup(".vimrc");

  clone(GH "walls", "Pictures", 0);

  cd(".config");

  // themes repo: some dirs go into ~/.config, the rest into per-user
  // locations (NixOS has no writable /usr/share).
  clone(GH "Themes", "themes", 0);
  run("mv themes/gtk-3.0 themes/Mousepad themes/Thunar themes/xfce4 .");
  run("cp -a themes/fonts/. '%s/.local/share/fonts/'", home);
  run("cp -a themes/themes/. '%s/.themes/'", home);
  run("cp -a themes/icons/. '%s/.icons/'", home);
  run("rm -rf themes");

  clone(GH "Qtile", "qtile", 0);

  // nvim: keep README, copy to root first, then pull vimrc/clang-format
  clone(GH "Neovim", "nvim", 1);
  run("cp -a nvim /root/.config/");
  run("mv nvim/clang-format '%s/.clang-format'", home);
  run("mv nvim/vimrc '%s/.vimrc'", home);

  clone(GH "Picom", "picom", 0);
  clone(GH "Oxwm", "oxwm", 0);
  clone(GH "terminals", "alacritty", 0);
  clone(GH "Tmux", "tmux", 0);

  // ---- shell -----------------------------------------------------------
  cd(home);
  backup(".bashrc");
  backup(".zshrc");

  clone(GH "Zshrc", "Documents/zsh", 0);
  cd("Documents/zsh");
  run("mv dircolors '%s/.local/bin/'", home);
  run("mv bash-completion zsh-autosuggestions zsh-syntax-highlighting "
      "'%s/.local/share/'",
      home);
  run("mv bashrc '%s/.bashrc'", home);
  run("mv zshrc '%s/.zshrc'", home);
  run("mv xiong-chiamiov.zsh-theme '%s/.zsh-theme/'", home);
  cd(home);
  run("rm -rf Documents/zsh");

  // ---- root's copy -----------------------------------------------------
  cd("/root");
  backup(".bashrc");
  backup(".zshrc");
  backup(".vimrc");
  run("mv .config/nvim/clang-format /root/.clang-format");
  run("mv .config/nvim/vimrc /root/.vimrc");

  // ---- ownership (only the user's files, not /etc/nixos or /root) ------
  run("chown -R '%s:' '%s'", user, home);

  printf(GREEN "\n==== Finished ====\n" RESET);
  return EXIT_SUCCESS;
}
