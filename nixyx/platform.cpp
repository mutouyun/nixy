#include "platform.h"

#include <QDebug>

#include "platform_base.h"
#include "platform_gcc.h"
#include "platform_vc.h"
#include "policy_vc8.h"
#include "policy_vc9.h"
#include "policy_vc10.h"
#include "policy_vc11.h"

//////////////////////////////////////////////////////////////////////////

struct Platform::Impl_
{
    Config& config_;
    PlatformBase* plat_;

    Impl_(Config& cfg)
        : config_(cfg), plat_(NULL)
    {
        if      (cfg.cc() == "vc8" || cfg.cc() == "vc08" || cfg.cc() == "vc2005")
            plat_ = new PlatformVC<PolicyVC8>;
        else if (cfg.cc() == "vc9" || cfg.cc() == "vc09" || cfg.cc() == "vc2008")
            plat_ = new PlatformVC<PolicyVC9>;
        else if (cfg.cc() == "vc10" || cfg.cc() == "vc2010")
            plat_ = new PlatformVC<PolicyVC10>;
        else if (cfg.cc() == "vc11" || cfg.cc() == "vc2012")
            plat_ = new PlatformVC<PolicyVC11>;
        else /* (cfg.cc() == "gcc") */
            plat_ = new PlatformGCC;
    }

    ~Impl_()
    {
        if (plat_) delete plat_;
    }

    bool make(void)
    {
        if (plat_)
            return plat_->make(config_);
        else
        {
            config_.setMessage("It is not supported on this setting.");
            return false;
        }
    }
};

//////////////////////////////////////////////////////////////////////////

Platform::Platform(Config& cfg)
    : pimpl_(new Impl_(cfg))
{}

Platform::~Platform()
{
    if (pimpl_) delete pimpl_;
}

//////////////////////////////////////////////////////////////////////////

bool Platform::make(void)
{
    Q_ASSERT(pimpl_);
    return pimpl_->make();
}
