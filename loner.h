#ifndef LONER_H
#define LONER_H

//#include <QMutex>
//#include <QScopedPointer>

template <typename T>
class Loner
{
protected:
    explicit Loner() = default; // Protected constructor to prevent direct instantiation
    ~Loner() = default; // Protected destructor to prevent direct deletion

public:
    static T& instance()
    {
        // Meyers' Singleton pattern with C++11 static initialization guarantee
        // The static local variable 'instance' is initialized only once,
        // and its initialization is thread-safe.
        static T instance;
        return instance;
    }

private:
    Loner(const Loner&) = delete; // Delete copy constructor to prevent copying
    Loner& operator=(const Loner&) = delete; // Delete assignment operator to prevent copying
};

#endif // LONER_H

