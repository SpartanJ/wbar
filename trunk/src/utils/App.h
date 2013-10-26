#ifndef _APP_H_
#define _APP_H_

#include <iostream>

class App {
public:

  App(std::string icon, std::string command, std::string title,
      unsigned long winid);
  ~App();

  std::string getIconName();
  std::string getCommand();
  std::string getTitle();
  unsigned long getWinid();

private:

  std::string icon;
  std::string command;
  std::string title;
  unsigned long winid;
};

#endif
