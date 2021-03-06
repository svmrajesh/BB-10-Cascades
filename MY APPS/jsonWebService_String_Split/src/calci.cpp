// Default empty project template
#include "calci.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/ListView>
#include <bb/cascades/ArrayDataModel>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <bb/data/JsonDataAccess>
#include <bb/cascades/QListDataModel>
 #include <QStringList>

bb::cascades::QVariantListDataModel listModel;

using namespace bb::cascades;
using namespace bb::data;

controller::controller(QObject *parent)
: QObject(parent)
	, m_succeeded(false)
    , m_active(false)
	, m_model(new GroupDataModel(QStringList()))

{

	m_model->setGrouping(ItemGrouping::None);
	QmlDocument* qml = QmlDocument::create("asset:///main.qml").parent(this);
	qml->setContextProperty("app", this);
	m_root = qml->createRootObject<AbstractPane>();

	Application::instance()->setScene(m_root);
}



bb::cascades::DataModel* controller::model() const
{
    return m_model;
}

bool controller::succeeded() const
{
    return m_succeeded;
}

bool controller::active() const
{
    return m_active;
}

QString controller::temperature() const
{
    return m_temperature;
}

QString controller::description() const
{
    return m_description;
}




void controller::sendRequest(const QString &CategoryName)
{

	if (m_active)
	        return;

	m_active = true;
		emit activeChanged();

	m_succeeded = false;


    QNetworkAccessManager* networkAccessManager = new QNetworkAccessManager(this);

    const QString queryUri = QString::fromLatin1("http://livetracking.entermobileapps.com/Mobile/Service1.svc/Mobile/CompanyRegistration?CompanyName=abc&HRName=new&Address=chn&Email=%@&Phoneno=12345&Password=12345&VehicleNumber=th12&VehicleModel=23&DevicID=123").arg(CategoryName);

    QNetworkRequest request(queryUri);

    QNetworkReply* reply = networkAccessManager->get(request);

    bool ok = connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

void controller::onFinished()
{

	m_succeeded = true;

	m_model->clear();
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	ArrayDataModel *model = new ArrayDataModel();

	QString response;
	if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
	{
		    JsonDataAccess jda;
	        QVariantMap map = jda.loadFromBuffer(reply->readAll()).toMap();

	        QVariantList addresses = map["RegistrationResult"].toList();
	        qDebug() << "RegistrationResult is " << map["RegistrationResult"].toString();
	        m_temperature = map["RegistrationResult"].toString();
	        m_temperature=m_temperature.section(':', 0, 0);

	        qDebug() << " New RegistrationResult is " <<m_temperature;
	        qDebug() <<map["RegistrationResult"].toString();


	        emit temperatureChanged();

	        QString result;
	        QString empid;
	        QString empid_no;
	        QString companyname;
	        QString::SectionFlag flag = QString::SectionSkipEmpty;

	        empid=m_temperature.section(':', 1, 1);
	        qDebug()<<m_temperature.section(':', 1, 1)<<"  =  "<<m_temperature.section(':', 2, 2);

	        m_description=m_temperature.section(':', 4, 4);
	        emit descriptionChanged();

	        model->append(addresses);
	        ListView *listView = m_root->findChild<ListView*>("listView");
	       	        listView->setDataModel(model);

	        // list: ["12", "14", "99", "231", "7"]
/*	        foreach(QVariant var, addresses) {
	            QVariantMap addressMap = var.toMap();

	            //qDebug() << "RegistrationResult is " << addressMap["RegistrationResult"].toString();
	            qDebug() << "CategoryID is " << addressMap["CategoryID"].toString();
	            qDebug() << "CategoryID is " << addressMap["ThumnailImage"].toUrl();

	            //m_temperature = addressMap["CategoryName"].toString();
	            emit temperatureChanged();
	            m_description = addressMap["CategoryID"].toString();
	            emit descriptionChanged();

	            //m_model->insert(addressMap);

	            model->append(addressMap);

	           // m_model->insertList(addresses.value<QVariantList>());
	        }
	        ListView *listView = m_root->findChild<ListView*>("listView");
	        listView->setDataModel(model);												*/
	    }
	    else {
	        qDebug() << "Server returned code " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	    }

		emit statusChanged();

	    m_active = false;
	    	emit activeChanged();
	}


