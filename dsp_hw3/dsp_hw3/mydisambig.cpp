#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <vector>
#include "Ngram.h"

using namespace std;

map<string, vector<string> > read_mapping(fstream &);
double bigram_prob(VocabString, VocabString, Ngram *, Vocab &);
double trigram_prob(VocabString, VocabString, VocabString, Ngram *, Vocab &);
void bi_viterbi(Ngram *, Vocab &, map<string, vector<string> >, fstream &);
void tri_viterbi(Ngram *, Vocab &, map<string, vector<string> >, fstream &);

int main(int argc, char *argv[])
{
    if (argc != 9) {
        fprintf(stderr, "\n*** The format should be like the following *** \n");
        fprintf(stderr, "./mydisambig -text $file -map $map -lm $LM -order $order > $ouput\n");
        fprintf(stderr, "*** These orders of four options can be arbitrary ***\n");
        return -1;
    }
    
    bool option[4] = {false, false, false, false};
    fstream test_file, map_file;
    char *lm_filename;
    int ngram_order;
    
    /* Read input arguments base on four different options */
    for (int i = 1; i < 9; i += 2) {
        if (!strcmp(argv[i], "-text")) {

            option[0] = true;
            test_file.open(argv[i+1], fstream::in);
            if (!test_file) {
                fprintf(stderr, "\n*** Test file doesn't exist. ***\n");
                return -1;
            }

        } else if (!strcmp(argv[i], "-map")) {

            option[1] = true;
            map_file.open(argv[i+1], fstream::in);
            if (!map_file) {
                fprintf(stderr, "\n*** Map file doesn't exist. ***\n");
                return -1;
            }

        } else if (!strcmp(argv[i], "-lm")) {

            option[2] = true;
            lm_filename = argv[i+1];

        } else if (!strcmp(argv[i], "-order")) {

            option[3] = true;
            ngram_order = atoi(argv[i+1]);

        } else {
            fprintf(stderr, "\n*** The format should be like the following *** \n");
            fprintf(stderr, "./mydisambig -text $file -map $map -lm $LM -order $order > $ouput\n");
            fprintf(stderr, "*** These orders of four options can be arbitrary ***\n");
            return -1;
        }
    }

    /* Check whether four options are encountered. */
    for (int i = 0; i < 4; i++) {
        if (!option[i]) {
            fprintf(stderr, "\n*** The format should be like the following *** \n");
            fprintf(stderr, "./mydisambig -text $file -map $map -lm $LM -order $order > $ouput\n");
            fprintf(stderr, "*** These orders of four options can be arbitrary ***\n");
            return 0;
        }
    }

    Vocab voc;
    Ngram *lm = new Ngram(voc, ngram_order);
    File *lm_file = new File(lm_filename, "r");
    lm->read(*lm_file);
    lm_file->close();

    map<string, vector<string> > zhuyin_big5 = read_mapping(map_file);


    if (ngram_order == 2)
        bi_viterbi(lm, voc, zhuyin_big5, test_file);
    else
        tri_viterbi(lm, voc, zhuyin_big5, test_file);

    return 0;
}

map<string, vector<string> > read_mapping(fstream &file)
{
    string line;
    map<string, vector<string> > mapping;
    vector<string> sequence;
    
    while (getline(file, line)) {
        sequence.clear();
        for (int i = 2; i < line.length(); i++) {
            if (line[i] != ' ') {
                sequence.push_back(line.substr(i, 2));
                i++;
            }
        }
        mapping[line.substr(0, 2)] = sequence;
    }

    return mapping;
}

double bigram_prob(VocabString w2, VocabString w1, Ngram *lm, Vocab &voc)
{
    VocabIndex wid1, wid2;

    wid1 = voc.getIndex(w1);
    if (wid1 == Vocab_None)
        wid1 = voc.getIndex(Vocab_Unknown);
    wid2 = voc.getIndex(w2);
    if (wid2 == Vocab_None)
        wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex content[] = {wid1, Vocab_None};
    
    return lm->wordProb(wid2, content);
}

double trigram_prob(VocabString w3, VocabString w1, VocabString w2, Ngram *lm, Vocab &voc)
{
    VocabIndex wid1, wid2, wid3;

    wid1 = voc.getIndex(w1);
    if (wid1 == Vocab_None)
        wid1 = voc.getIndex(Vocab_Unknown);
    wid2 = voc.getIndex(w2);
    if (wid2 == Vocab_None)
        wid2 = voc.getIndex(Vocab_Unknown);
    wid3 = voc.getIndex(w3);
    if (wid3 == Vocab_None)
        wid3 = voc.getIndex(Vocab_Unknown);
        
    VocabIndex content[] = {wid2, wid1, Vocab_None};
    
    return lm->wordProb(wid3, content);
}

void bi_viterbi(Ngram *lm, Vocab &voc, map<string, vector<string> > mapping, fstream &file)
{
    string line;
    vector<string> sentence;
    int len;
    
    // Bigram
    while (getline(file, line)) {
        
        sentence.clear();
        // Calculate sequence length and transform sequence into string variables per character
        for (int i = 0; i < line.length(); i++) {
            if (line[i] != ' ') {
                sentence.push_back(line.substr(i, 2));
                i++;
            }
        }
        len = sentence.size();
        
        vector<map<string, double> > delta;
        vector<map<string, string> > parent;
        vector<string> previous;
        vector<string> candidate;
        map<string, double> candidate_prob;
        map<string, string> candidate_parent;
        double current_prob;
        string current_parent;

        for (int i = 0; i < len; i++) {
            
            candidate_prob.clear();
            candidate_parent.clear();
            candidate.clear();
            
            /* Store all the candidates */
            for (int j = 0; j < mapping[sentence[i]].size(); j++)
                candidate.push_back(mapping[sentence[i]][j]);
            
            if (i == 0) {
                /* First character */
                /* P = P(w1 | <s>) */
                for (int j = 0; j < candidate.size(); j++) {
                    candidate_prob[candidate[j]] = bigram_prob(candidate[j].c_str(), Vocab_SentStart, lm, voc);
                    candidate_parent[candidate[j]] = Vocab_SentStart;
                    previous.push_back(candidate[j]);
                }
                delta.push_back(candidate_prob);
                parent.push_back(candidate_parent);
            } else {
                vector<string> tmp_previous;
                
                /* candidates in time i */
                for (int j = 0; j < candidate.size(); j++) {
                    
                    current_prob = -2147483647;
                    current_parent.clear();
                    
                    /* Determine each cadidate's parent from time i-1*/
                    for (int k = 0; k < previous.size(); k++) {
                        float temp = bigram_prob(candidate[j].c_str(), previous[k].c_str(), lm, voc);
                        float tail = 0;
                        /* The probability of the candidate being the last character : P(</s> | character) */
                        if (i == len-1)
                            tail = bigram_prob(Vocab_SentEnd, candidate[j].c_str(), lm, voc);
                        
                        if (current_prob < (temp + tail + delta[i-1][previous[k]])) {
                            current_prob = temp + tail + delta[i-1][previous[k]];
                            current_parent = previous[k];
                        }
                    }
                    candidate_prob[candidate[j]] = current_prob;
                    candidate_parent[candidate[j]] = current_parent;
                    tmp_previous.push_back(candidate[j]);
                }

                previous.clear();
                previous = tmp_previous;
                delta.push_back(candidate_prob);
                parent.push_back(candidate_parent);
            }
        }
        
        current_prob = -2147483647;
        /* Find the maximum prob. of the time T */
        for (map<string, double>::iterator it = delta[len-1].begin(); it != delta[len-1].end(); it++) {
            if (current_prob < it->second) {
                current_parent = it->first;
                current_prob = it->second;
            }
        }

        sentence.clear();
        sentence.insert(sentence.begin(), Vocab_SentEnd);
        sentence.insert(sentence.begin(), " ");
        sentence.insert(sentence.begin(), current_parent);

        for (int i = len-1; i >= 0; i--) {
            sentence.insert(sentence.begin(), " ");
            sentence.insert(sentence.begin(), parent[i][current_parent]);
            current_parent = parent[i][current_parent];
        }
        
        for (int i = 0; i < sentence.size(); i++)
            cout << sentence[i];
        cout << endl;
    }
}

void tri_viterbi(Ngram *lm, Vocab &voc, map<string, vector<string> > mapping, fstream &file)
{
    string line;
    vector<string> sentence;
    int len;
    
    // Trigram
    while (getline(file, line)) {
        
        sentence.clear();
        // Calculate sequence length and transform sequence into string variables per character
        for (int i = 0; i < line.length(); i++) {
            if (line[i] != ' ') {
                sentence.push_back(line.substr(i, 2));
                i++;
            }
        }
        len = sentence.size();
        
        vector<map<pair<string, string>, double> > delta;
        vector<map<pair<string, string>, string> > parent;
        vector<pair<string, string> > previous;
        vector<string> candidate;
        map<pair<string, string>, double> candidate_prob;
        map<pair<string, string>, string> candidate_parent;
        double current_prob;
        string current_parent_w1, current_parent_w2;

        for (int i = 0; i < len; i++) {

            candidate_prob.clear();
            candidate_parent.clear();
            candidate.clear();
            
            /* Store all the candidates */
            for (int j = 0; j < mapping[sentence[i]].size(); j++)
                candidate.push_back(mapping[sentence[i]][j]);
            
            if (i == 0) {
                /* First character */
                /* P = P(w1 | <s>) */
                pair<string, string> index;
                for (int j = 0; j < candidate.size(); j++) {
                    index = make_pair("<s>", candidate[j]);
                    candidate_prob[index] = bigram_prob(candidate[j].c_str(), Vocab_SentStart, lm, voc);
                    candidate_parent[index] = "";
                    previous.push_back(index);
                }
                delta.push_back(candidate_prob);
                parent.push_back(candidate_parent);
            } else {
                vector<pair<string, string> > tmp_previous;

                /* candidates in time i */
                for (int j = 0; j < candidate.size(); j++) {
                    
                    current_prob = -2147483647;
                    current_parent_w1.clear();
                    current_parent_w2.clear();

                    /* Determine each cadidate's parent from time i-1*/
                    for (int k = 0; k < previous.size(); k++) {
                        float temp = trigram_prob(candidate[j].c_str(), previous[k].first.c_str(), previous[k].second.c_str(), lm, voc);
                        float tail = 0;
                        /* The probability of the candidate being the last character : P(</s> | character) */
                        if (i == len-1)
                            tail = trigram_prob(Vocab_SentEnd, previous[k].second.c_str(), candidate[j].c_str(), lm, voc) + bigram_prob(Vocab_SentEnd, candidate[j].c_str(), lm, voc);
                        
                        if (current_prob < (temp + tail + delta[i-1][previous[k]])) {
                            current_prob = temp + tail + delta[i-1][previous[k]];
                            current_parent_w1 = previous[k].first;
                            current_parent_w2 = previous[k].second;
                        }
                    }
                    pair<string, string> index;
                    index = make_pair(current_parent_w2, candidate[j]);

                    candidate_prob[index] = current_prob;
                    candidate_parent[index] = current_parent_w1;
                    tmp_previous.push_back(index);
                }

                previous.clear();
                previous = tmp_previous;
                delta.push_back(candidate_prob);
                parent.push_back(candidate_parent);
            }
        }
        
        current_prob = -2147483647;
        /* Find the maximum prob. of the time T */
        for (map<pair<string, string>, double>::iterator it = delta[len-1].begin(); it != delta[len-1].end(); it++) {
            if (current_prob < it->second) {
                current_prob = it->second;
                current_parent_w1 = it->first.first;
                current_parent_w2 = it->first.second;
            }
        }

        sentence.clear();
        sentence.insert(sentence.begin(), Vocab_SentEnd);
        sentence.insert(sentence.begin(), " ");
        sentence.insert(sentence.begin(), current_parent_w2);
        sentence.insert(sentence.begin(), " ");
        sentence.insert(sentence.begin(), current_parent_w1);
        
        pair<string, string> index;

        for (int i = len-1; i >= 1; i--) {
            sentence.insert(sentence.begin(), " ");
            index = make_pair(current_parent_w1, current_parent_w2);
            sentence.insert(sentence.begin(), parent[i][index]);
            current_parent_w2 = current_parent_w1;
            current_parent_w1 = parent[i][index];
        }
        
        for (int i = 0; i < sentence.size(); i++)
            cout << sentence[i];
        
        cout << endl;
        
    }
}
