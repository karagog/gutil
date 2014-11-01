/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <gutil/exception.h>
#include <QPluginLoader>


#if defined(Q_OS_LINUX)
    #define PLUGIN_PREFIX "lib"
    #define PLUGIN_SUFFIX ".so"
#elif defined(Q_OS_WIN)
    #define PLUGIN_PREFIX ""
    #define PLUGIN_SUFFIX ".dll"
#endif


NAMESPACE_GUTIL1(Qt);


/** A utility to simplify loading plugins at runtime. */
class PluginUtils
{
    GUTIL_STATIC_CLASS(PluginUtils);
public:

    /** Loads the plugin with the given filename and path.  You provide a plugin loader, and
     *  the instance of the interface returned will be valid until you call unload() on the plugin loader.
     *
     *  \param filename The filename of the plugin, excluding any lib or .so prefix/suffix.
     *  \param plugin_dir_path The path to the plugins directory.  If this parameter is empty
     *  it will search in the working directory.
     *  \returns A pointer to the desired interface of the plugin. It will always return valid, or
     *  throw an exception on error.
    */
    template<class IFace>static IFace *LoadPlugin(QPluginLoader &pl, const QString &filename, const QString &plugin_dir_path = QString())
    {
        QString plugin_filename = QString("%1%2%3%4")
                .arg(plugin_dir_path.isEmpty() ? "" : (plugin_dir_path + "/"))
                .arg(PLUGIN_PREFIX)
                .arg(filename)
                .arg(PLUGIN_SUFFIX);
        pl.setFileName(plugin_filename);
        if(!pl.instance()){
            QString error = pl.errorString();
            throw Exception<>(QString("Cannot load plugin: %1").arg(error).toUtf8().constData());
        }

        IFace *ret = qobject_cast<IFace *>(pl.instance());
        if(0 == ret)
            throw Exception<>("Plugin lacking interface");
        return ret;
    }

};


END_NAMESPACE_GUTIL1;

#endif // PLUGINLOADER_H
