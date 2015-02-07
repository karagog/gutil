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

#ifndef GUTIL_NO_NETWORK_FUNCTIONALITY

#include "updatenotifier.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
USING_NAMESPACE_GUTIL;
using namespace std;

NAMESPACE_GUTIL1(Qt);


UpdateNotifier::UpdateNotifier(const Version &current_version, QObject *parent)
    :QObject(parent),
      m_currentVersion(current_version)
{}

void UpdateNotifier::CheckForUpdates(const QUrl &version_url)
{
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(_reply_received(QNetworkReply*)));

    nam->get(QNetworkRequest(version_url));
}

void UpdateNotifier::_reply_received(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        // Follow the redirects
        QVariant redirect_url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if(!redirect_url.isNull()){
            if(reply->url() == redirect_url.toUrl()){
                reply->manager()->deleteLater();
                return;
            }
            reply->manager()->get(QNetworkRequest(redirect_url.toUrl()));
            return;
        }

        QByteArray version = reply->readLine().trimmed();
        QByteArray update_url = reply->readLine().trimmed();
        emit UpdateInfoReceived(QString::fromUtf8(version), QString::fromUtf8(update_url));
    }

    reply->manager()->deleteLater();
}


END_NAMESPACE_GUTIL1;

#endif // GUTIL_NO_NETWORK_FUNCTIONALITY
