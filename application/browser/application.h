// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_APPLICATION_BROWSER_APPLICATION_H_
#define XWALK_APPLICATION_BROWSER_APPLICATION_H_

#include <map>
#include <set>
#include <string>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_vector.h"
#include "base/observer_list.h"
#include "xwalk/application/browser/event_observer.h"
#include "xwalk/application/common/application_data.h"
#include "xwalk/runtime/browser/runtime.h"

namespace xwalk {

class RuntimeContext;

namespace application {

class ApplicationHost;
class Manifest;

// The Application class is representing an active (running) application.
// Application instances are owned by ApplicationService.
// ApplicationService will delete an Application instance when it is
// terminated.
// FIXME(Mikhail): Add information about the relationship of the Application
// and the render process.
class Application : public Runtime::Observer {
 public:
  ~Application();
  // Returns the unique application id which is used to distinguish the
  // application amoung both running applications and installed ones
  // (ApplicationData objects).
  std::string id() const { return application_data_->ID(); }
  class Observer {
   public:
    // Invoked when application is terminated - all its pages (runtimes)
    // are closed.
    virtual void OnApplicationTerminated(Application* app) {}

   protected:
    virtual ~Observer() {}
  };

  // Closes all the application's runtimes (application pages).
  void Close();

  // Returns Runtime (application page) containing the application's
  // 'main document'. The main document is the main entry point of
  // the application to the system. This method will return 'NULL'
  // if application has different entry point (local path manifest key).
  // See http://anssiko.github.io/runtime/app-lifecycle.html#dfn-main-document
  // The main document never has a visible window on its own.
  Runtime* GetMainDocumentRuntime() const { return main_runtime_; }

  const std::set<Runtime*>& runtimes() const { return runtimes_; }

  const ApplicationData* data() const { return application_data_; }
  ApplicationData* data() { return application_data_; }

 private:
  // Runtime::Observer implementation.
  virtual void OnRuntimeAdded(Runtime* runtime) OVERRIDE;
  virtual void OnRuntimeRemoved(Runtime* runtime) OVERRIDE;

  // We enforce ApplicationService ownership.
  friend class ApplicationService;
  Application(scoped_refptr<ApplicationData> data,
              RuntimeContext* context,
              Observer* observer);
  bool Launch();

  friend class FinishEventObserver;
  bool RunMainDocument();
  bool RunFromLocalPath();
  void CloseMainDocument();
  bool IsOnSuspendHandlerRegistered(const std::string& app_id) const;

  RuntimeContext* runtime_context_;
  scoped_refptr<ApplicationData> application_data_;
  Runtime* main_runtime_;
  std::set<Runtime*> runtimes_;
  scoped_ptr<EventObserver> finish_observer_;
  Observer* observer_;

  DISALLOW_COPY_AND_ASSIGN(Application);
};

}  // namespace application
}  // namespace xwalk

#endif  // XWALK_APPLICATION_BROWSER_APPLICATION_H_
