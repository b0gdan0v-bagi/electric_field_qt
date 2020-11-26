#include <QObject>
#include <QDebug>

class Foo: public QObject {
    Q_OBJECT
public:
    explicit Foo(QObject *parent = nullptr): QObject(parent) {

    }

    void foo() {
        QList<int> list;

        list.append(77);
        list.append(13);
        list.append(10);

        emit dataReady(list);
    }

signals:
    void dataReady(const QList<int> &list);
};

class Boo : public QObject {
    Q_OBJECT
public:
    explicit Boo(QObject* parent = nullptr) : QObject(parent) {
    }

public slots:
    void boo(const QList<int>& list) {
        qDebug() << "from boo: " << list;
    }
};

class Moo : public QObject {
    Q_OBJECT
public:
    explicit Moo(QObject* parent = nullptr) : QObject(parent) {
    }

public slots:
    void moo(const QList<int>& list) {
        qDebug() << "from moo: " << list;
    }
};