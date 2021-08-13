#ifndef __SIMPLEMESSAGE_EV_H__
#define __SIMPLEMESSAGE_EV_H__

class simplemessage_ev : public QObject
{
  Q_OBJECT
  
public:
  Q_SIGNAL void c_add(std::string id, std::string name);
  Q_SIGNAL void c_remove(std::string id);
};

#endif
