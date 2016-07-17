#include "qslpassword.h"

#include <QBuffer>
#include <QDataStream>
#include <QFile>

using namespace qsl;
using namespace std;

Password::Password(const QByteArray &password, const PasswordDefaults &defaults)
	: _defaults(defaults)
{
	_hash = genhash(password, _salt);
}

Password::Password(const QByteArray &password, PasswordDefaults &&defaults)
	: _defaults(defaults)
{
	_hash = genhash(password, _salt);
}

Password::Password(string password, const PasswordDefaults &defaults)
	: _defaults(defaults)
{
	_hash = genhash(password.data(), _salt);
}

Password::Password(string password, PasswordDefaults &&defaults)
	: _defaults(defaults)
{
	_hash = genhash(password.data(), _salt);
}

Password::Password(const char *password, const PasswordDefaults &defaults)
	: _defaults(defaults)
{
	_hash = genhash(password, _salt);
}

Password::Password(const char *password, PasswordDefaults &&defaults)
	: _defaults(defaults)
{
	_hash = genhash(password, _salt);
}

QByteArray Password::gensalt(uint16_t len)
{
	QScopedPointer<QFile> in(new QFile("/dev/urandom"));
	if (!in->open(QIODevice::ReadOnly))
	{
		in.reset(new QFile("/dev/random"));
		if (!in->open(QIODevice::ReadOnly))
		{
			fprintf(stderr, "QSLPassword: failed to open random device, can't generate salt\n");
			return QByteArray();
		}
	}
	return in->read(len);
}

QByteArray Password::genhash(const QByteArray &password, QByteArray &salt)
{
	if (salt.isEmpty() && _defaults.saltLength != 0)
	{
		salt = gensalt(_defaults.saltLength);
		if (salt.length() != _defaults.saltLength)
		{
			fprintf(stderr, "QSLPassword: unable to get salt, will hash without salt\n");
			salt.clear();
		}
	}
	
	QByteArray hash = password;
	for (uint i = 0; i < _defaults.repeats; i++)
		hash = QCryptographicHash::hash(salt + hash, _defaults.algorithm);
	return hash;
}

bool Password::operator== (const QByteArray &password)
{
	QByteArray salt = _salt;
	QByteArray hash = genhash(password, salt);
	return (hash == _hash);
}

QByteArray Password::entry() const
{
	QByteArray e;
	QBuffer buf(&e);
	buf.open(QIODevice::WriteOnly);
	QDataStream out(&buf);
	out.setByteOrder(QDataStream::BigEndian);
	out.setVersion(QDataStream::Qt_5_5);
	
	out << _defaults.repeats;
	out << (int16_t)_defaults.algorithm;
	out << _salt;
	out << _hash;
	
#ifdef CMAKE_DEBUG
	printf("Serialized Password with:\n"
		   "   REPEATS:   %d\n"
		   "   ALGORITHM: %d\n"
		   "   SALT:      %s\n"
		   "   HASH:      %s\n",
		   _defaults.repeats, (int16_t)_defaults.algorithm,
		   _salt.toBase64(QByteArray::KeepTrailingEquals).data(),
		   _hash.toBase64(QByteArray::KeepTrailingEquals).data());
#endif
	
	buf.close();
	return e.toBase64(QByteArray::KeepTrailingEquals);
}

Password::Password(const PasswordEntry &entry)
{
	QByteArray e = QByteArray::fromBase64(entry.entry);
	QBuffer buf(&e);
	buf.open(QIODevice::ReadOnly);
	QDataStream in(&buf);
	in.setByteOrder(QDataStream::BigEndian);
	in.setVersion(QDataStream::Qt_5_5);
	
	in >> _defaults.repeats;
	int16_t algo = 0;
	in >> algo;
	_defaults.algorithm = (QCryptographicHash::Algorithm) algo;
	in >> _salt;
	in >> _hash;
	
#ifdef CMAKE_DEBUG
	printf("Deserialized Password with:\n"
		   "   REPEATS:   %d\n"
		   "   ALGORITHM: %d\n"
		   "   SALT:      %s\n"
		   "   HASH:      %s\n",
		   _defaults.repeats, (int16_t)_defaults.algorithm,
		   _salt.toBase64(QByteArray::KeepTrailingEquals).data(),
		   _hash.toBase64(QByteArray::KeepTrailingEquals).data());
#endif
	
	buf.close();
}
