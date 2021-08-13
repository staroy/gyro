#ifndef __simplemessage_h__
#define __simplemessage_h__

/*!
 * \file simplemessage.h
 * 
 * \brief Header file that declares simple_wallet class.
 */

#pragma once

#include "crypto/crypto.h"

#include <string>

class QGuiApplication;
class simplemessage_ev;

namespace cryptonote {

  class simplemessage
  {
    QGuiApplication *p_App;
    simplemessage_ev *p_Ev;

    std::string path_;
    crypto::secret_key sec_;
    std::string self_id_;
    std::string self_name_;
    std::vector<std::pair<std::string, std::string>> cc_;

  public:
    void run();
    void stop();

    void set_path(const std::string& v) { path_ = v; }
    void set_cipher_key(const crypto::secret_key& v) { sec_ = v; }
    void set_self_id(const std::string& id) { self_id_ = id; }
    void set_self_name(const std::string& name) { self_name_ = name; }
    void c_initialize(const std::vector<std::pair<std::string, std::string>>& cc);
    void c_add(const std::string& id, const std::string& name);
    void c_remove(const std::string& id);
  };
}

#endif
