#ifndef __data__
#define __data__

class data {
    public:
        data();
        ~data();

        void save_to_file();

        void *get_bytes();

        int size();
};

#endif