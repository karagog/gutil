/*Copyright 2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_UPDATENOTIFIER_H
#define GUTIL_UPDATENOTIFIER_H

#ifndef GUTIL_NO_NETWORK_FUNCTIONALITY

#include <gutil/version.h>
#include <QUrl>
#include <QObject>

class QNetworkReply;

namespace GUtil{ namespace Qt{


/** Implements a background thread that checks for
 *  updates by parsing a version string at a URL.
 *
 *  The format of the version string is as follows:
 *  It is 2 lines, the first line is the version (i.e. 1.0.0)
 *  and the second line is a URL where the user can download
 *  the latest version.
*/
class UpdateNotifier : public QObject
{
    Q_OBJECT
    GUtil::Version m_currentVersion;
public:
    UpdateNotifier(const GUtil::Version &current_version, QObject * = 0);

    GUtil::Version const &GetCurrentVersion() const{ return m_currentVersion; }

    /** Starts a background thread to check the given URL
     *  for updates. The signal "UpdateInfoReceived" will be
     *  emitted when the thread is finished, regardless if
     *  there is an update or not.
    */
    void CheckForUpdates(const QUrl &version_url);

signals:
    /** Emitted from the background thread when it knows
     *  whether or not you need an update
    */
    void UpdateInfoReceived(const QString &latest_version_string,
                            const QUrl &update_location);

private slots:
    void _reply_received(QNetworkReply *);
};


}}

#endif // GUTIL_NO_NETWORK_FUNCTIONALITY

#endif // GUTIL_UPDATENOTIFIER_H
