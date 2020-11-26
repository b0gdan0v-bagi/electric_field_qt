#include <QObject>

class Foo : public QObject {
    Q_OBJECT
public:
    explicit Foo(QObject* parent = nullptr) : QObject(parent) {

    }

    void foo() {
        QList<int> list;

        list.append(77);
        list.append(13);
        list.append(10);

        emit dataReady(list);
    }

signals:
    void dataReady(const QList<int>& list);
};