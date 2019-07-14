#ifndef SIZE_H
#define SIZE_H

struct Size
{
    int x;
    int y;

    Size(int x, int y) : x(x), y(y) {}
    Size(const Size& other) : Size(other.x, other.y) {}

    Size& operator=(const Size& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    bool operator==(const Size& other)
    {
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(const Size& other)
    {
        return !(*this == other);
    }
};

#endif // SIZE_H
