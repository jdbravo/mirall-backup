/*
 * Copyright (C) by Krzesimir Nowak <krzesimir@endocode.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include "creds/dummycredentials.h"
#include "mirall/mirallaccessmanager.h"

namespace Mirall
{

void DummyCredentials::syncContextPreInit(CSYNC*)
{}

void DummyCredentials::syncContextPreStart(CSYNC*)
{}

bool DummyCredentials::changed(AbstractCredentials* credentials) const
{
    DummyCredentials* dummy(dynamic_cast< DummyCredentials* >(credentials));

    return dummy == 0;
}

QString DummyCredentials::authType() const
{
    return QString::fromLatin1("dummy");
}

QString DummyCredentials::user() const
{
    return QString();
}

QNetworkAccessManager* DummyCredentials::getQNAM() const
{
    return new MirallAccessManager;
}

bool DummyCredentials::ready() const
{
    return true;
}

bool DummyCredentials::stillValid(QNetworkReply *reply)
{
    Q_UNUSED(reply)
    return true;
}

bool DummyCredentials::fetchFromUser(Account *account)
{
    Q_UNUSED(account)
    return false;
}

void DummyCredentials::fetch(Account*)
{
    Q_EMIT(fetched());
}

void DummyCredentials::persist(Account*)
{}

} // ns Mirall
