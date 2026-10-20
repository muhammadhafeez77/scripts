// LAST :: PROGRESS BAR
//  include themes.git in this script

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

// 77//================== Declerations ===================

// change "coffee" to your username , must be same here and configuration.nix
// or any other config that has that username.
char PATH[] = "/home/coffee";
char END[] = "sudo chown -R coffee:coffee /home/coffee";

char command[500];
void cd(char path[]);
void killgit();

int main(int argc, char *argv[])
{

  (void)argc;
  if (argv[1] == NULL)
  {
    argv[1] = argv[0];
  }
  // 77// ================= Last Stand =====================

  if (strcmp(argv[1], "--execute") != 0)
  {
    fprintf(stderr, RED "\n usage :  %s  [--execute] to execute  ", argv[0]);
    fprintf(stderr, "\n usage : Run as [ root ]  ");
    fprintf(stderr, "\n must have : git  \n\n" RESET);
    exit(0);
  }

  printf("Are you sure you want to continue:[y,n]  : ");

  if (scanf("%c", command) == 1 && (command[0] != 'y' && command[0] != 'Y'))
  {
    fprintf(stderr, RED "\n i hate you res  \n\n");
    fprintf(stderr, "\n\n\t ==== Ending Execution =====\n  " RESET);
    exit(0);
  }

  // 77// ================= Execution  =====================
  //
  printf(GREEN "\n\n\t ==== Begin Execution =====\n  " RESET);
  fprintf(stderr, RED "\n\n i hate you res  \n" RESET);

  cd(PATH);
  system("mkdir -p /root/.config");
  mkdir("Downloads", 0755);
  mkdir("Documents", 0755);
  snprintf(command, sizeof(command),
           " git clone https://github.com/muhammadhafeez77/Nixos-Flakes.git"
           "./nixos-dotfiles ");
  system(command);

  cd("nixos-dotfiles");
  killgit();
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
  killgit();
  cd("..");

  cd(".config");

  system("git clone https://github.com/muhammadhafeez77/Themes ./themes");
  cd("themes");
  killgit();
  system("mv gtk-3.0 Mousepad Thunar xfce4 ../");
  system("mv fonts /usr/share/fonts");
  system("mv themes /usr/share/themes");
  system("mv icons /usr/share/icons");
  cd("..");
  system("rm -rf themes");

  system("git clone https://github.com/muhammadhafeez77/Qtile ./qtile");
  cd("qtile");
  killgit();
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/Neovim ./nvim");
  system("cp -r ./nvim /root/.config/");
  cd("nvim");
  system("mv clang-format ../../.clang-format");
  system("mv vimrc ../../.vimrc");
  system("mv README.md CONFIG.md");
  killgit();
  system("mv CONFIG.md README.md");
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/Picom ./picom");
  cd("picom");
  killgit();
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/Oxwm ./oxwm");
  cd("oxwm");
  killgit();
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/terminals ./alacritty");
  cd("alacritty");
  killgit();
  cd("..");

  system("git clone https://github.com/muhammadhafeez77/Tmux ./tmux");
  cd("tmux");
  killgit();

  cd(PATH);
  system("mv .bashrc ._backup__bashrc");
  system("mv .zshrc ._backup__zshrc");
  mkdir(".zsh-theme", 0755);

  cd("Documents");
  system("git clone https://github.com/muhammadhafeez77/Zshrc ./zsh");
  cd("zsh");
  killgit();

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
  killgit();

  system(END);
  printf(GREEN "\n\n\t ==== Finished =====\n  " RESET);
  return EXIT_SUCCESS;
}

void cd(char local_path[])
{
  // local_path[strcspn(local_path, "\n")] == '\0';
  if (strlen(local_path) > 1)
  {
    snprintf(command, sizeof(command), "cd %s", local_path);
    system(command);
  }
}
void killgit()
{
  system("rm -rf .git");
  system("rm .gitignore README.md");
}
