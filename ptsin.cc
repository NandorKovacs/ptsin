#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>

std::string help_message() {
  std::ostringstream oss;
  oss << "Usage:" << std::endl;
  oss << "ptsin [-nhi] <device> <input>" << std::endl;
  oss << "-h: prints this help message" << std::endl;
  oss << "-n: sends a carriage return after sending the desired input. ignored "
         "if -i is also given"
      << std::endl;
  oss << "-i: interactive." << std::endl;
  return oss.str();
}

std::string bad_param(std::string s) {
  std::ostringstream oss;
  oss << "No parameter \"" << s << "\" doesn't exist" << std::endl;
  oss << help_message();
  return oss.str();
}

struct Settings {
  bool interactive = false;
  bool carriage_return = false;
};

void send_cr(int open_result) {
  char newline = '\r';
  ioctl(open_result, TIOCSTI, &newline);
}

void send_str(int open_result, std::string s) {
  for (char c : s) {
    ioctl(open_result, TIOCSTI, &c);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << help_message();
    return 1;
  }

  Settings settings;

  int arg_i = 1;
  while (arg_i < argc && argv[arg_i][0] == '-') {
    std::string s = argv[arg_i];

    if (s.size() == 1) {
      std::cout << bad_param(s);
      return 1;
    }

    for (int i = 1; i < s.size(); ++i) {
      if (s[i] == 'h') {
        std::cout << help_message();
        return 0;
      }
      if (s[i] == 'n') {
        settings.carriage_return = true;
        continue;
      }
      if (s[i] == 'i') {
        settings.interactive = true;
        continue;
      }
      std::cout << bad_param(s);
      return 1;
    }
    ++arg_i;
  }
  


int open_result = open(argv[arg_i], O_RDWR);
++arg_i;
if (open_result == -1) {
  std::cout << "can not open " << argv[arg_i] << std::endl;
  return 1;
}

if (settings.interactive) {
  std::string s;
  while (std::getline(std::cin, s)) {
    send_str(open_result, s);
    send_cr(open_result);
  }
}

std::string command;
for (int i = arg_i; i < argc; ++i) {
  command = command + " " + argv[i];
}

send_str(open_result, command);
if (settings.carriage_return) {
  send_cr(open_result);
}

close(open_result);
return 0;
}