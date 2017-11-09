#ifndef BITMAP_H
#define BITMAP_H

template <typename T>
class Bitmap {

public:

    class iterator {

    public:

        enum PARCOURS_MODE {
            HORIZONTAL_MODE,
            VERTICAL_MODE,
            REVERSE_HORIZONTAL_MODE,
            REVERSE_VERTICAL_MODE
        };

    private:

        Bitmap * bitmap;
        int i, j;
        PARCOURS_MODE mode;

    public:

        iterator() : i(0), j(0), bitmap(0) {}
        iterator(Bitmap * b, int i = 0, int j = 0, PARCOURS_MODE mode = HORIZONTAL_MODE) : i(i), j(j), bitmap(b), mode(mode) {}
        iterator(const iterator& it) : i(it.i), j(it.j), bitmap(it.bitmap) {}

        T * operator*() { return bitmap ? (&bitmap->at((unsigned int)i, (unsigned int)j)) : 0; }
        const T * operator*() const { return bitmap ? (&bitmap->at((unsigned int)i, (unsigned int)j)) : 0; }

        T * operator->() { return bitmap ? (&bitmap->at((unsigned int)i, (unsigned int)j)) : 0; }
        const T * operator->() const { return bitmap ? (&bitmap->at((unsigned int)i, (unsigned int)j)) : 0; }

        T& operator[](unsigned int x) {
            switch (mode) {
            case HORIZONTAL_MODE: return bitmap->at(i, j + x);
            case VERTICAL_MODE: return bitmap->at(i + x, j);
            case REVERSE_HORIZONTAL_MODE: return bitmap->at(i, j - x);
            case REVERSE_VERTICAL_MODE: return bitmap->at(i - x, j);
            }
            return bitmap->at(i, j + x);
        }
        const T& operator[](unsigned int x) const {
            switch (mode) {
            case HORIZONTAL_MODE: return bitmap->at(i, j + x);
            case VERTICAL_MODE: return bitmap->at(i + x, j);
            case REVERSE_HORIZONTAL_MODE: return bitmap->at(i, j - x);
            case REVERSE_VERTICAL_MODE: return bitmap->at(i - x, j);
            }
            return bitmap->at(i, j + x);
        }

        iterator& operator=(const iterator& it) {
            bitmap = it.bitmap;
            i = it.i;
            j = it.j;
            mode = it.mode;
            return *this;
        }

        bool operator==(const iterator& it) const { return (bitmap == it.bitmap && i == it.i && j == it.j); }
        bool operator!=(const iterator& it) const { return (bitmap != it.bitmap || i != it.i || j != it.j); }

        iterator& operator++() {
            switch (mode) {
            case HORIZONTAL_MODE:
                j = (j + 1) % bitmap->width();
                if (j == 0) i++;
                break;
            case VERTICAL_MODE:
                i = (i + 1) % bitmap->height();
                if (i == 0) j++;
                break;
            case REVERSE_HORIZONTAL_MODE:
                j--;
                if (j == 0) {
                    i--;
                    j = bitmap->width() - 1;
                }
                break;
            case REVERSE_VERTICAL_MODE:
                i--;
                if (i == 0) {
                    j--;
                    i = bitmap->height() - 1;
                }
                break;
            }
            return *this;
        }

        iterator& operator--() {
            switch (mode) {
            case HORIZONTAL_MODE:
                j--;
                if (j == 0) {
                    i--;
                    j = bitmap->width() - 1;
                }
                break;
            case VERTICAL_MODE:
                i--;
                if (i == 0) {
                    j--;
                    i = bitmap->height() - 1;
                }
                break;
            case REVERSE_HORIZONTAL_MODE:
                j = (j + 1) % bitmap->width();
                if (j == 0) i++;
                break;
            case REVERSE_VERTICAL_MODE:
                i = (i + 1) % bitmap->height();
                if (i == 0) j++;
                break;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator it(*this);
            switch (mode) {
            case HORIZONTAL_MODE:
                it.j = (it.j + 1) % bitmap->width();
                if (it.j == 0) it.i++;
                break;
            case VERTICAL_MODE:
                it.i = (it.i + 1) % bitmap->height();
                if (it.i == 0) it.j++;
                break;
            case REVERSE_HORIZONTAL_MODE:
                it.j--;
                if (it.j == 0) {
                    it.i--;
                    it.j = bitmap->width() - 1;
                }
                break;
            case REVERSE_VERTICAL_MODE:
                it.i--;
                if (it.i == 0) {
                    it.j--;
                    it.i = bitmap->height() - 1;
                }
                break;
            }
            return it;
        }

        iterator operator--(int) {
            iterator it(*this);
            switch (mode) {
            case HORIZONTAL_MODE:
                it.j--;
                if (it.j == 0) {
                    it.i--;
                    it.j = bitmap->width() - 1;
                }
                break;
            case VERTICAL_MODE:
                it.i--;
                if (it.i == 0) {
                    it.j--;
                    it.i = bitmap->height() - 1;
                }
                break;
            case REVERSE_HORIZONTAL_MODE:
                it.j = (it.j + 1) % bitmap->width();
                if (it.j == 0) it.i++;
                break;
            case REVERSE_VERTICAL_MODE:
                it.i = (it.i + 1) % bitmap->height();
                if (it.i == 0) it.j++;
                break;
            }
            return it;
        }

        iterator operator+(int x) const {
            iterator it(*this);
            int tmp;
            switch (mode) {
            case HORIZONTAL_MODE:
                tmp = i * bitmap->width() + j + x;
                break;
            case VERTICAL_MODE:
                tmp = (i + x) * bitmap->width() + j;
                break;
            case REVERSE_HORIZONTAL_MODE:
                tmp = i * bitmap->width() + j - x;
                break;
            case REVERSE_VERTICAL_MODE:
                tmp = (i - x) * bitmap->width() + j;
                break;
            }
            it.i = tmp / bitmap->width();
            it.j = tmp % bitmap->width();
            if (it.j < 0)
                it.j += bitmap->width();
            return it;
        }
        
        iterator operator-(int x) const {
            return this->operator+(-x);
        }

        int operator-(const iterator& it) const {
            switch (mode) {
            case HORIZONTAL_MODE: return (it.i - i) * bitmap->width() + (it.j - j);
            case VERTICAL_MODE: return (it.j - j) * bitmap->height() + (it.i - i);
            case REVERSE_HORIZONTAL_MODE: return (i - it.i) * bitmap->width() + (j - it.j);
            case REVERSE_VERTICAL_MODE: return (j - it.j) * bitmap->height() + (i - it.i);
            }
            return false;
        }

    };
    
    class const_iterator {
        
    public:

        enum PARCOURS_MODE {
            HORIZONTAL_MODE,
            VERTICAL_MODE,
            REVERSE_HORIZONTAL_MODE,
            REVERSE_VERTICAL_MODE
        };

    private:

        const Bitmap * bitmap;
        int i, j;
        PARCOURS_MODE mode;

    public:

        const_iterator() : i(0), j(0), bitmap(0) {}
        const_iterator(const Bitmap * b, int i = 0, int j = 0, PARCOURS_MODE mode = HORIZONTAL_MODE) : i(i), j(j), bitmap(b), mode(mode) {}
        const_iterator(const const_iterator& it) : i(it.i), j(it.j), bitmap(it.bitmap) {}

        const T * operator*() const { return bitmap ? (&bitmap->at((unsigned int)i, (unsigned int)j)) : 0; }
        const T * operator->() const { return bitmap ? (&bitmap->at((unsigned int)i, (unsigned int)j)) : 0; }

        const T& operator[](unsigned int x) const {
            switch (mode) {
            case HORIZONTAL_MODE: return bitmap->at(i, j + x);
            case VERTICAL_MODE: return bitmap->at(i + x, j);
            case REVERSE_HORIZONTAL_MODE: return bitmap->at(i, j - x);
            case REVERSE_VERTICAL_MODE: return bitmap->at(i - x, j);
            }
            return bitmap->at(i, j + x);
        }
        
        const_iterator& operator=(const const_iterator& it) {
            bitmap = it.bitmap;
            i = it.i;
            j = it.j;
            mode = it.mode;
            return *this;
        }
        
        bool operator==(const const_iterator& it) const { return (bitmap == it.bitmap && i == it.i && j == it.j); }
        bool operator!=(const const_iterator& it) const { return (bitmap != it.bitmap || i != it.i || j != it.j); }

        const_iterator& operator++() {
            switch (mode) {
            case HORIZONTAL_MODE:
                j = (j + 1) % bitmap->width();
                if (j == 0) i++;
                break;
            case VERTICAL_MODE:
                i = (i + 1) % bitmap->height();
                if (i == 0) j++;
                break;
            case REVERSE_HORIZONTAL_MODE:
                j--;
                if (j == 0) {
                    i--;
                    j = bitmap->width() - 1;
                }
                break;
            case REVERSE_VERTICAL_MODE:
                i--;
                if (i == 0) {
                    j--;
                    i = bitmap->height() - 1;
                }
                break;
            }
            return *this;
        }

        const_iterator& operator--() {
            switch (mode) {
            case HORIZONTAL_MODE:
                j--;
                if (j == 0) {
                    i--;
                    j = bitmap->width() - 1;
                }
                break;
            case VERTICAL_MODE:
                i--;
                if (i == 0) {
                    j--;
                    i = bitmap->height() - 1;
                }
                break;
            case REVERSE_HORIZONTAL_MODE:
                j = (j + 1) % bitmap->width();
                if (j == 0) i++;
                break;
            case REVERSE_VERTICAL_MODE:
                i = (i + 1) % bitmap->height();
                if (i == 0) j++;
                break;
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator it(*this);
            switch (mode) {
            case HORIZONTAL_MODE:
                it.j = (it.j + 1) % bitmap->width();
                if (it.j == 0) it.i++;
                break;
            case VERTICAL_MODE:
                it.i = (it.i + 1) % bitmap->height();
                if (it.i == 0) it.j++;
                break;
            case REVERSE_HORIZONTAL_MODE:
                it.j--;
                if (it.j == 0) {
                    it.i--;
                    it.j = bitmap->width() - 1;
                }
                break;
            case REVERSE_VERTICAL_MODE:
                it.i--;
                if (it.i == 0) {
                    it.j--;
                    it.i = bitmap->height() - 1;
                }
                break;
            }
            return it;
        }

        const_iterator operator--(int) {
            const_iterator it(*this);
            switch (mode) {
            case HORIZONTAL_MODE:
                it.j--;
                if (it.j == 0) {
                    it.i--;
                    it.j = bitmap->width() - 1;
                }
                break;
            case VERTICAL_MODE:
                it.i--;
                if (it.i == 0) {
                    it.j--;
                    it.i = bitmap->height() - 1;
                }
                break;
            case REVERSE_HORIZONTAL_MODE:
                it.j = (it.j + 1) % bitmap->width();
                if (it.j == 0) it.i++;
                break;
            case REVERSE_VERTICAL_MODE:
                it.i = (it.i + 1) % bitmap->height();
                if (it.i == 0) it.j++;
                break;
            }
            return it;
        }
        
        const_iterator operator+(int x) const {
            const_iterator it(*this);
            int tmp;
            switch (mode) {
            case HORIZONTAL_MODE:
                tmp = i * bitmap->width() + j + x;
                break;
            case VERTICAL_MODE:
                tmp = (i + x) * bitmap->width() + j;
                break;
            case REVERSE_HORIZONTAL_MODE:
                tmp = i * bitmap->width() + j - x;
                break;
            case REVERSE_VERTICAL_MODE:
                tmp = (i - x) * bitmap->width() + j;
                break;
            }
            it.i = tmp / bitmap->width();
            it.j = tmp % bitmap->width();
            if (it.j < 0)
                it.j += bitmap->width();
            return it;
        }
        
        const_iterator operator-(int x) const {
            return this->operator+(-x);
        }
        
        int operator-(const const_iterator& it) const {
            switch (mode) {
            case HORIZONTAL_MODE: return (it.i - i) * bitmap->width() + (it.j - j);
            case VERTICAL_MODE: return (it.j - j) * bitmap->height() + (it.i - i);
            case REVERSE_HORIZONTAL_MODE: return (i - it.i) * bitmap->width() + (j - it.j);
            case REVERSE_VERTICAL_MODE: return (j - it.j) * bitmap->height() + (i - it.i);
            }
            return false;
        }

    };

private:

    unsigned int w, h;
    T * d;

public:

    Bitmap() : w(0), h(0), d(0) {}
    Bitmap(unsigned int width, unsigned int height) : w(height ? width : 0), h(width ? height : 0) {
        d = (w > 0 && h > 0) ? new T[w * h] : 0;
    }
    Bitmap(const Bitmap& b) : w(b.w), h(b.h) {
        d = (w > 0 && h > 0) ? new T[w * h] : 0;
    }
    ~Bitmap() {
        if (d) delete[] d;
    }

    unsigned int width() const { return w; }
    unsigned int height() const { return h; }

    iterator begin() { return iterator(this); }
    const_iterator begin() const { return const_iterator(this); }
    iterator end() { return iterator(this, 0, (int)h); }
    const_iterator end() const { return const_iterator(this, 0, (int)h); }
    iterator begin_line(unsigned int i) { return iterator(this, (int)i); }
    const_iterator begin_line(unsigned int i) const { return const_iterator(this, (int)i); }
    iterator end_line(unsigned int i) { return iterator(this, (int)i + 1); }
    const_iterator end_line(unsigned int i) const { return const_iterator(this, (int)i + 1); }
    iterator begin_column(unsigned int j) { return iterator(this, 0, (int)j, iterator::VERTICAL_MODE); }
    const_iterator begin_column(unsigned int j) const { return const_iterator(this, 0, (int)j, const_iterator::VERTICAL_MODE); }
    iterator end_column(unsigned int j) { return iterator(this, 0, (int)j + 1, iterator::VERTICAL_MODE); }
    const_iterator end_column(unsigned int j) const { return const_iterator(this, 0, (int)j + 1, const_iterator::VERTICAL_MODE); }

    T& at(unsigned int i, unsigned int j) { return d[i * w + j]; }
    const T& at (unsigned int i, unsigned int j) const { return d[i * w + j]; }

    iterator operator[](int i) { return iterator(this, (int)i); }
    const_iterator operator[](int i) const { return const_iterator(this, (int)i); }

    T * data() { return d; }
    const T * data() const { return d; }

};

#endif