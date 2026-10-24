// LAST :: PROGRESS BAR
// command -v git >/dev/null 2>&1 && echo 1 || echo 0 // check if git is or not

#include <linux/limits.h>
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
#define GH "git clone --depth 1 https://github.com/muhammadhafeez77/"

// 77//================== Declerations ===================

// change "coffee" to your username , must be same here and configuration.nix
// or any other config that has that username.
static char USER[255];
static char PATH[PATH_MAX];

char command[4095];
void cd(const char *local_path);
void clone(const char *url, const char *dest, const int readme);
static int exists(const char *local_path);
static void die(const char *core);
static void run(const char *core, ...) __attribute__((format(printf, 1, 2)));
static void run(const char *core, ...);
static void backup(const char *core);
static void auth();
// 77//================= Main ============================

int main(int argc, char *argv[])
{

  // 77// ================= Last Stand =====================

  if (argc < 2 || strcmp(argv[1], "--execute") != 0)
  {
    printf(RED "\n usage :  %s  [--execute] <username> to execute  ", argv[0]);
    fprintf(stderr, "\n usage : Run as [ root ]  ");
    fprintf(stderr, "\n must have : git  \n\n" RESET);
    die("add --execute as flag");
  }
  auth();

  // 77// ================= Execution  =====================
  //
  printf(GREEN "\n\n\t ==== Begin Execution =====\n  " RESET);
  printf(RED "\n\n i hate you res  \n" RESET);

  cd(PATH);
  system("mkdir -p /root/.config");
  mkdir("Downloads", 0755);
  mkdir("Documents", 0755);
  snprintf(command, sizeof(command),
           " git clone https://github.com/muhammadhafeez77/Nixos-Flakes.git"
           "./nixos-dotfiles ");
  system(command);
  clone(GH "themes", "themes", 0);
  cd("nixos-dotfiles");
  system(
      "cp /etc/nixos/hardware-configuration.nix ./hardware-configuration.nix");
  system(" mv /etc/nixos/ /etc/backup_nixos ");
  cd(PATH);
  snprintf(command, sizeof(command), "mv %s/nixos-dotfiles /etc/nixos", PATH);
  system(command);
  system("ln -s /etc/nixos ./nixos-dotfiles");

  printf("\n\n\t ===== INSTALLING CONFIGS =====\n\n");
  cd(PATH);
  system("rm .vimrc");
  system("mkdir -p .config");

  system("git clone https://github.com/muhammadhafeez77/walls ./Pictures");
  cd("Pictures");
  cd("..");

  cd(".config");

  system("git clone https://github.com/muhammadhafeez77/Themes ./themes");
  cd("themes");
  system("mv gtk-3.0 Mousepad Thunar xfce4 ../");
  system("mv fonts /usr/share/fonts");
  system("mv themes /usr/share/themes");
  system("mv icons /usr/share/icons");
  cd("..");
  system("rm -rf themes");

  system("git clone https://github.com/muhammadhafeez77/Qtile ./qtile");
  cd("qtile");
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/Neovim ./nvim");
  system("cp -r ./nvim /root/.config/");
  cd("nvim");
  system("mv clang-format ../../.clang-format");
  system("mv vimrc ../../.vimrc");
  system("mv README.md CONFIG.md");
  system("mv CONFIG.md README.md");
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/Picom ./picom");
  cd("picom");
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/Oxwm ./oxwm");
  cd("oxwm");
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/terminals ./alacritty");
  cd("alacritty");
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/Tmux ./tmux");
  cd("tmux");

  cd(PATH);
  system("mv .bashrc ._backup__bashrc");
  system("mv .zshrc ._backup__zshrc");
  mkdir(".zsh-theme", 0755);

  cd("Documents");
  system("git clone https://github.com/muhammadhafeez77/Zshrc ./zsh");
  cd("zsh");

  system("mv dircolors /usr/bin/");
  system("mv bash-completion zsh-autosuggestions zsh-syntax-highlighting "
         "/usr/share/");
  system("mv bashrc ../../.bashrc");
  system("mv zshrc ../../.zshrc");
  system("mv xiong-chiamiov.zsh-theme ../../.zsh-theme/");
  cd("..");
  system("rm -rf zsh");

  cd("/root");
  system("mv .bashrc ._backup__bashrc");
  system("mv .zshrc ._backup__zshrc");
  system("mv .vimrc ._backup__vimrc ");

  cd(".config/nvim");
  system("mv clang-format ../../.clang-format");
  system("mv vimrc ../../.vimrc");

  system("do chown");
  printf(GREEN "\n\n\t ==== Finished =====\n  " RESET);
  return EXIT_SUCCESS;
}

// 77// ==================== Functions ==========================

static void auth()
{
  if (getuid() != 0)
  {
    fprintf(stderr, "\n usage : Run as [ root ]  ");
    die(" are u an idiot ? ");
  }
  memset(command, 0, sizeof(command));

  printf("Are you sure you want to continue:[y,n]  : ");

  if (scanf("%c", command) == 1 && (command[0] != 'y' && command[0] != 'Y'))
  {
    printf(RED "\n i hate you res  \n\n");
    die(" Execution Cancelled");
  }
}
static void backup(const char *core)
{
  if (!exists(core))
    return;

  memset(command, 0, sizeof(command));
  snprintf(command, sizeof(command), "%s.bak.%ld ", core, (long)time(NULL));
  printf(YELLOW " + backup %s -> %s " RESET, core, command);

  if (rename(core, command))
  {
    perror(core);
    die(" renaming failed");
  }
}

void clone(const char *url, const char *dest, const int readme) // 0 = delete it
{
  run(" '%s'  '%s' ", url, dest);
  run("rm -rf '%s/.git' '%s.gitignore ", dest, dest);
  if (!readme)
  {
    run("rm README.md");
  }
}

static void run(const char *core, ...)
{
  memset(command, 0, sizeof(command));

  va_list temp;
  va_start(temp, core);
  int n = vsnprintf(command, sizeof(command), core, temp);
  va_end(temp);

  if (n < 0 || (size_t)n >= sizeof(command))
    die("Buffer overflow ");

  int check = system(command);
  if (check != 0)
    die(command);
}

static void die(const char *core)
{
  fprintf(stderr, RED "\n\n ERROR : { %s } \n\n", core);
  printf("\n\n\t ==== Ending Execution =====\n  " RESET);
  exit(EXIT_FAILURE);
}

static int exists(const char *local_path)
{
  struct stat st;
  return lstat(local_path, &st) == 0;
}

void cd(const char *local_path)
{
  if (chdir(local_path) != 0)
  {
    perror(local_path);
    exit(EXIT_FAILURE);
  }
}
