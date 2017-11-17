#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <cmath>
#include <iostream>
#include <array>
#include <vector>
#include <bitset>
#include <memory>
#include <algorithm>
#include <iomanip>

template <int N = 8>
class Huffman {

    // Frequency node
    class Node {
        
    public:

        // Frequency counter
        float freq;
        
        // Value (null if node)
        std::unique_ptr<std::bitset<N>> value;

        // Childs
        std::unique_ptr<Node> left, right;
        
    private:

        // Default constructor
        Node() : freq(0.0f) {}

    public:

        // Leave constructor
        Node(unsigned int v, float f) {
            freq = f;
            value = std::move(std::unique_ptr<std::bitset<N>>(new std::bitset<N>(v)));
        }

        // Node constructor
        Node(Node * l, Node * r) {
            left = std::move(std::unique_ptr<Node>(l));
            right = std::move(std::unique_ptr<Node>(r));
            freq = l->freq + r->freq;
        }

        // Indicate if node is leave
        inline bool leave() const {
            return (bool)value;
        }

        // Console output displaying
        void display(std::vector<bool>& bv) {
            if (leave())
                std::cout << value->to_ulong() << "\t" << std::setprecision(4) << freq << "\t\t" << bv << std::endl;
            else {
                bv.push_back(0);
                left->display(bv);
                bv.back() = 1;
                right->display(bv);
                bv.pop_back();
            }
        }

        void fill_codes(std::array<std::vector<bool>, 1 << N>& codes, std::vector<bool>& bv) {
            if (leave())
                codes[value->to_ulong()] = bv;
            else {
                bv.push_back(0);
                left->fill_codes(codes, bv);
                bv.back() = 1;
                right->fill_codes(codes, bv);
                bv.pop_back();
            }
        }

        // Write the tree content
        void write(std::vector<bool>& stream, unsigned int elem_size = N) {
            if (leave()) {
                stream.push_back(1);
                for (unsigned int i = 0; i < elem_size; i++)
                    stream.push_back(value->operator[](i));
            }
            else {
                stream.push_back(0);
                left->write(stream, elem_size);
                right->write(stream, elem_size);
            }
        }

        // Read the tree content
        static Node * read(const std::vector<bool>& stream, unsigned int& it, unsigned int elem_size = N) {
            Node * node = new Node();
            if (stream[it++] == 1) {
                node->value = std::move(std::unique_ptr<std::bitset<N>>(new std::bitset<N>()));
                for (unsigned int i = 0; i < elem_size; i++)
                    node->value->operator[](i) = stream[it++];
            }
            else {
                node->left = std::move(std::unique_ptr<Node>(Huffman<N>::Node::read(stream, it, elem_size)));
                node->right = std::move(std::unique_ptr<Node>(Huffman<N>::Node::read(stream, it, elem_size)));
            }
            return node;
        }

        // Greater comparison
        static bool greater(Node * a, Node * b) {
            return a->freq > b->freq;
        }

    };

    // Frequency tree
    std::unique_ptr<Node> ftree;

    // Codes array
    std::array<std::vector<bool>, 1 << N> codes;

    // Reload codes
    void reload_codes() {
        for (unsigned int i = 0; i < (1 << N); i++)
            codes[i].clear();
        std::vector<bool> bv;
        ftree->fill_codes(codes, bv);
    }
    
public:

    // Create a frequency tree
    void create(const void * data, std::size_t count, unsigned int elem_size = N) {
        std::array<unsigned int, 1 << N> freqs;
        for (unsigned int i = 0; i < (1 << N); i++)
            freqs[i] = 0;
        for (unsigned int i = 0; i < count; i++) {
            std::bitset<N> elem;
            for (unsigned int j = 0; j < elem_size; j++) {
                unsigned long nbB = (unsigned long)i * (unsigned long)N + (unsigned long)j;
                elem[j] = (((unsigned char *)data)[(unsigned int)(nbB / (unsigned long)8)] >> (nbB % 8)) & 0x1;
            }
            freqs[elem.to_ulong()]++;
        }
        std::vector<Node *> nodes;
        for (unsigned int i = 0; i < (1 << N); i++) {
            if (freqs[i] != 0)
                nodes.push_back(new Node(i, (float)freqs[i] / (float)count));
        }
        while (nodes.size() > 1) {
            std::sort(nodes.begin(), nodes.end(), Node::greater);
            Node * r = nodes.back();
            nodes.pop_back();
            Node * l = nodes.back();
            nodes.pop_back();
            nodes.push_back(new Node(l, r));
        }
        ftree = std::move(std::unique_ptr<Node>(nodes.front()));
        reload_codes();
    }

    // Console output displaying
    void display() {
        std::cout << "Value\tFrequency\tCode" << std::endl << std::endl;
        std::vector<bool> bv;
        ftree->display(bv);
    }

    // Write frequency tree
    unsigned int write(std::vector<bool>& stream, unsigned int elem_size = N) {
        unsigned int sz = stream.size();
        ftree->write(stream, elem_size);
        return stream.size() - sz;
    }

    // Write data content with frequency tree
    unsigned int write(std::vector<bool>& stream, const void * data, std::size_t count) {
        unsigned int sz = stream.size();
        for (unsigned int i = 0; i < count; i++) {
            std::bitset<N> elem;
            for (unsigned int j = 0; j < N; j++) {
                unsigned long nbB = (unsigned long)i * (unsigned long)N + (unsigned long)j;
                elem[j] = (((unsigned char *)data)[(unsigned int)(nbB / (unsigned long)8)] >> (nbB % 8)) & 0x1;
            }
            std::vector<bool>& code = codes[elem.to_ulong()];
            for (unsigned int i = 0, sz = code.size(); i < sz; i++)
                stream.push_back(code[i]);
        }
        return sz;
    }

    // Read frequency tree
    unsigned int read(const std::vector<bool>& stream, unsigned int elem_size = N) {
        unsigned int it = 0;
        ftree = std::move(std::unique_ptr<Node>(Huffman<N>::Node::read(stream, it, elem_size)));
        reload_codes();
        return it;
    }

    // Read data content with frequency tree
    unsigned int read(const std::vector<bool>& stream, void * data, std::size_t count) {
        unsigned int it = 0;
        for (unsigned int i = 0; i < count; i++) {
            std::reference_wrapper<Node> current = *ftree;
            while (!current.get().leave()) {
                if (stream[it++] == 0)
                    current = *current.get().left;
                else
                    current = *current.get().right;
            }
            for (unsigned int j = 0; j < N; j++) {
                unsigned long nbB = (unsigned long)i * (unsigned long)N + (unsigned long)j;
                unsigned int block = (unsigned int)(nbB / (unsigned long)8);
                unsigned int bitpos = (nbB % 8);
                ((unsigned char *)data)[block] &= ~(0x1 << bitpos);
                ((unsigned char *)data)[block] |= ((current.get().value->operator[](j) ? 0x1 : 0x0) << bitpos);
            }
        }
        return it;
    }

};

#endif // HUFFMAN_H