#ifndef NETWORK_REQUEST_H
#define NETWORK_REQUEST_H

#include <QObject>
#include <QNetworkRequest>

namespace Network
{
	class Request
	{
	public:
		Request(const QString& address = QString());

		QString address() const;
		void setAddress(const QString& address);

		void addParam(const QString& name, const QVariant& value);
		bool removeParam(const QString& name);

		QStringList paramsNames() const;
		QMap<QString, QString> params() const;

		QUrl url(bool withParams = true) const;
		QNetworkRequest request(bool withParams = true) const;
		QByteArray data() const;

	private:
		QString _address;
		QMap<QString, QString> _params;
	};
}

#endif // NETWORK_REQUEST_H
