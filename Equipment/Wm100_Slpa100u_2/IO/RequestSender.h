#ifndef NETWORK_REQUESTSENDER_H
#define NETWORK_REQUESTSENDER_H

#include "request.h"
#include <QNetworkProxy>
#include <QEventLoop>

namespace Network
{
	class RequestSender : public QObject
	{
		Q_OBJECT
	public:

		enum RequestError
		{
			NoError,
			TimeoutError
		};

        RequestSender(qint64 maxWaitTime = 10000);
		~RequestSender();

		void setProxy(const QNetworkProxy& proxy);

		QByteArray get(Request& request);
		QByteArray post(Request& request);
		QByteArray getWhileSuccess(Request& request, int maxCount = 2);
		QByteArray postWhileSuccess(Request& request, int maxCount = 2);
		
		void setMaxWaitTime(qint64 max);

		qint64 maxWaitTime() const;
		RequestError error() const;
    private:
		QByteArray sendRequest(Request& request, bool getRequest = true);
		QByteArray sendWhileSuccess(Request& request, int maxCount = 2, bool getRequest = true);

	private:
		qint64 _maxWaitTime;
		RequestError _error;
		QNetworkProxy _proxy;
        //QEventLoop loop;
        //QEventLoop loop1;

	};
}


#endif // NETWORK_REQUESTSENDER_H
