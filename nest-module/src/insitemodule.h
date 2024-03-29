#ifndef INSITE_MODULE_H
#define INSITE_MODULE_H

// Includes from sli:
#include "slifunction.h"
#include "slimodule.h"

// Put your stuff into your own namespace.
namespace insite {

/**
 * Class defining your model.
 * @note For each model, you must define one such class, with a unique name.
 */
class InsiteModule : public SLIModule {
 public:
  // Interface functions ------------------------------------------

  /**
   * @note The constructor registers the module with the dynamic loader.
   *       Initialization proper is performed by the init() method.
   */
  InsiteModule();

  /**
   * @note The destructor does not do much in modules.
   */
  ~InsiteModule();

  /**
   * Initialize module.
   * @param SLIInterpreter* SLI interpreter
   */
  void init(SLIInterpreter*);

  /**
   * Return the name of your model.
   */
  const std::string name(void) const;
};

}  // namespace insite

#endif
