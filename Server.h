#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QFile>
#include <QTextStream>

class Server : public QTcpServer {
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    bool startServer(quint16 port);
    void saveScoresToFile() {
        QFile file("scores.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Failed to open scores file for writing.";
            return;
        }

        QTextStream out(&file);
        for (const auto &key : scores.keys()) {
            out << key << ":" << scores[key] << "\n";
        }
        file.close();
    }

    void loadScoresFromFile() {
        QFile file("scores.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Failed to open scores file for reading.";
            return;
        }

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(":");
            if (parts.size() == 2) {
                QString playerName = parts[0];
                int score = parts[1].toInt();
                scores[playerName] = score;
            }
        }
        file.close();
    }
protected:
    void handleNewConnection();
    //void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void readClient();
    void disconnectClient();

private:
    QMap<QTcpSocket*, QString> clients;
    QMap<QString, int> scores;

    void sendScoreboard();
    void printScoreboard() {
        qDebug() << "Current Scoreboard:";
        for (auto it = scores.begin(); it != scores.end(); ++it) {
            qDebug() << it.key() << ":" << it.value();
        }
    }
};

#endif // SERVER_H
