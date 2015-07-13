/**
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */

#include <ctime>
#include <iostream>

#include "modsecurity/modsecurity.h"
#include "src/config.h"
#include "src/unique_id.h"


namespace ModSecurity {

/**
 * @name    ModSecurity
 * @brief   Initilizes ModSecurity CPP API
 *
 * ModSecurity initializer.
 *
 * Example Usage:
 * @code
 *
 * using ModSecurity::ModSecurity;
 *
 * ModSecurity *msc = new ModSecurity();
 *
 * @endcode
 */
ModSecurity::ModSecurity()
    : m_connector("") {
    UniqueId::uniqueId();
    srand(time(NULL));
}


ModSecurity::~ModSecurity() {
}


/**
 * @name    whoAmI
 * @brief   Return information about this ModSecurity version and platform.
 *
 * Platform and version are two questions that community will ask prior to
 * provide support. Making it available internally and to the connector as
 * well.
 *
 * @note This information maybe will be used by a log parser. If you want to
 *       update it, make it in a fashion that won't break the existent parsers.
 *       (e.g. adding extra information _only_ to the end of the string)
 */
std::string ModSecurity::whoAmI() {
    std::string platform("Unknown platform");

#if AIX
    platform = "AIX";
#elif LINUX
    platform = "Linux";
#elif OPENBSD
    platform = "OpenBSD";
#elif SOLARIS
    platform = "Solaris";
#elif HPUX
    platform = "HPUX";
#elif MACOSX
    platform = "MacOSX";
#elif FREEBSD
    platform = "FreeBSD";
#elif NETBSD
    platform = "NetBSD";
#elif WIN32
    platform = "Windows";
#endif

    return std::string("ModSecurity v" MODSECURITY_VERSION \
        " (" + platform + ")");
}


/**
 * @name    setConnectorInformation
 * @brief   Set information about the connector that is using the library.
 *
 * For the purpose of log it is necessary for modsecurity to understand which
 * 'connector' is consuming the API.
 *
 * @note It is strongly recommended to set a information in the following
 *       pattern:
 *
 *       ConnectorName vX.Y.Z-tag (something else)
 *
 *       For instance: ModSecurity-nginx v0.0.1-alpha (Whee)
 *
 * @param connector Information about the connector.
 *
 */
void ModSecurity::setConnectorInformation(std::string connector) {
    m_connector = connector;
}


/**
 * @name    getConnectorInformation
 * @brief   Returns the connector information.
 *
 * Returns whatever was set by 'setConnectorInformation'. Check
 * setConnectorInformation documentation to understand the expected format.
 *
 * @retval "" Nothing was informed about the connector.
 * @retval !="" Connector information.
 */
const std::string& ModSecurity::getConnectorInformation() {
    return m_connector;
}


/**
 * @name    msc_set_connector_info
 * @brief   Set information about the connector that is using the library.
 *
 * For the purpose of log it is necessary for modsecurity to understand which
 * 'connector' is consuming the API.
 *
 * @note It is strongly recommended to set a information in the following
 *       pattern:
 *
 *       ConnectorName vX.Y.Z-tag (something else)
 *
 *       For instance: ModSecurity-nginx v0.0.1-alpha (Whee)
 *
 * @param connector Information about the connector.
 *
 */
extern "C" void msc_set_connector_info(ModSecurity *msc,
    const char *connector) {
    msc->setConnectorInformation(std::string(connector));
}


/**
 * @name    msc_who_am_i
 * @brief   Return information about this ModSecurity version and platform.
 *
 * Platform and version are two questions that community will ask prior to
 * provide support. Making it available internally and to the connector as
 * well.
 *
 * @note This information maybe will be used by a log parser. If you want to
 *       update it, make it in a fashion that won't break the existent parsers.
 *       (e.g. adding extra information _only_ to the end of the string)
 */
extern "C" const char *msc_who_am_i(ModSecurity *msc) {
    return msc->whoAmI().c_str();
}


/**
 * @name    msc_cleanup
 * @brief   Cleanup ModSecurity C API
 *
 * Cleanup ModSecurity instance.
 *
 */
extern "C" void msc_cleanup(ModSecurity *msc) {
    delete msc;
}


/**
 * @name    msc_init
 * @brief   Initilizes ModSecurity C API
 *
 * ModSecurity initializer.
 *
 * Example Usage:
 * @code
 *
 * ModSecurity msc = msc_init();
 *
 * @endcode
 */
extern "C" ModSecurity *msc_init() {
    ModSecurity *modsec = new ModSecurity();

    return modsec;
}

}  // namespace ModSecurity