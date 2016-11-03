#pragma once

/// @file qslpassword.h This file contains the `qsl::Password` class that is used
/// to serialize, deserialize, hash and compare passwords. These passwords can be
/// stored in a table as a string and later be compared or updated. It is not
/// possible to restore the password and this is intentional.

#include "qslnamespace.h"

#include <QCryptographicHash>

namespace qsl {

/**
 * This struct stores default values for password hashing.
 */
struct QSL_PUBLIC PasswordDefaults
{
	/// The number of hash operations that should be performed.
	uint16_t repeats = 1000;
	/// The hash algorithm to use.
	QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha512;
	/// The salt length to use. Set this to 0 to hash without a salt.
	uint16_t saltLength = 32;
};

/**
 * This struct is used to de-ambiguos the constructor.
 */
struct QSL_PUBLIC PasswordEntry { /** The entry. */ QByteArray entry; };

/**
 * This class is used to store a password. It can hash a given password or
 * read one from the database and check it against a given plain password.
 */
class QSL_PUBLIC Password
{
public:
	/// Generates a salt from /dev/(u)random of the given length.
	static QByteArray gensalt(uint16_t len);
	
	/// Hash the given password using the given default values.
	Password(const QByteArray &password, const PasswordDefaults &defaults);
	/// Hash the given password using the given default values.
	Password(const QByteArray &password, PasswordDefaults &&defaults = PasswordDefaults());
	/// Hash the given password using the given default values.
	Password(std::string password, const PasswordDefaults &defaults);
	/// Hash the given password using the given default values.
	Password(std::string password, PasswordDefaults &&defaults = PasswordDefaults());
	/// Hash the given password using the given default values.
	Password(const char *password, const PasswordDefaults &defaults);
	/// Hash the given password using the given default values.
	Password(const char *password, PasswordDefaults &&defaults = PasswordDefaults());
	/// Read the hash, salt and defaults from the given database entry.
	Password(const PasswordEntry &entry);
	
	/// Returns an entry for the database.
	QByteArray entry() const;
	
	/// Check the hash against the given plain password.
	bool operator== (const QByteArray &password);
	/// Check the hash against the given plain password.
	bool operator== (const std::string &password) { return (*this == QByteArray(password.data())); }
	
private:
	QByteArray genhash(const QByteArray &password, QByteArray &salt);
	
	PasswordDefaults _defaults;
	QByteArray _salt;
	QByteArray _hash;
};

}
