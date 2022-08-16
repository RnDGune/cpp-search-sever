#pragma once

#include "search_server.h"
#include "process_queries.h"

using namespace std;

void PrintDocument(const Document& document) {
    std::cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << endl;
}

void Test_1() {
    SearchServer search_server("and with"sv);

    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
        ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }

    const string query = "curly and funny -not"s;
    {
        const auto [words, status] = search_server.MatchDocument("dog"s, 1);
        cout << words.size() << " words dog for document 1 "s << endl;
    }
    {
        auto search_result = search_server.FindTopDocuments("funny"s, DocumentStatus::ACTUAL);
        for (auto tmp : search_result) {
            PrintDocument(tmp);
        }
    }
    {
        auto search_result = search_server.FindTopDocuments("funny"s, DocumentStatus::IRRELEVANT);
        for (auto tmp : search_result) {
            PrintDocument(tmp);
        }
    }
    {
        search_server.AddDocument(6, "funny pet and nasty rat"sv, DocumentStatus::IRRELEVANT, { 1, 2 });
        auto search_result = search_server.FindTopDocuments("funny"s, DocumentStatus::IRRELEVANT);
        for (auto tmp : search_result) {
            PrintDocument(tmp);
            cout << "221" << endl;
        }
    }
    {
        cout << "Even ids:"s << endl;
        for (const Document& document : search_server.FindTopDocuments("funny and nasty"s, [](int document_id, DocumentStatus status, int rating)
            { return document_id % 2 == 0; }))
        {
            PrintDocument(document);
        }
    }

}

void Test_2() {
    SearchServer search_server("and with"s);
    const string query = "curly and funny -not"s;

    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
        ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }

    {
        const auto [words, status] = search_server.MatchDocument(query, 1);
        cout << words.size() << " words for document 1"s << endl;
        // 1 words for document 1
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::seq, query, 2);
        cout << words.size() << " words for document 2"s << endl;
        // 2 words for document 2
    }
    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 1);
        cout << words.size() << " words for document 1"s << endl;
        // 1 words for document 1
    }


    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 2);
        cout << words.size() << " words for document 2"s << endl;
        // 2 words for document 2
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 3);
        cout << words.size() << " words for document 3"s << endl;
        // 0 words for document 3
    }

}

void Test_3() {
    SearchServer search_server("and with"s);
    const string query = "curly and funny -not"s;

    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
        ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }

    auto report = [&search_server, &query] {
        cout << search_server.GetDocumentCount() << " documents total, "s
            << search_server.FindTopDocuments(query).size() << " documents for query ["s << query << "]"s << endl;
    };

    report();
    // однопоточная версия
    search_server.RemoveDocument(5);
    report();
    // однопоточная версия
    search_server.RemoveDocument(execution::seq, 1);
    report();
    // многопоточная версия
    search_server.RemoveDocument(execution::par, 2);
    report();
}

void Test_4() {
    SearchServer search_server("and with"s);

    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
        ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }


    const vector<string> queries = {
        "nasty rat -not"s,
        "not very funny nasty pet"s,
        "curly hair"s
    };
    id = 0;
    for (
        const auto& documents : ProcessQueries(search_server, queries)
        ) {
        cout << documents.size() << " documents for query ["s << queries[id++] << "]"s << endl;
    }
    //3 documents for query [nasty rat -not]
    //5 documents for query[not very funny nasty pet]
    //    2 documents for query[curly hair]
}

void Test_5() {
    SearchServer search_server("and with"s);

    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
        ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }

    const vector<string> queries = {
        "nasty rat -not"s,
        "not very funny nasty pet"s,
        "curly hair"s
    };
    for (const Document& document : ProcessQueriesJoined(search_server, queries)) {
        cout << "Document "s << document.id << " matched with relevance "s << document.relevance << endl;
    }
}

void Test_6() {
    SearchServer search_server("funny pet and nasty rat"s);
    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet and nasty rat"s,
            "funny pet and nasty rat"s,
            "funny pet and nasty rat"s,
        }
        ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
    }
    {
        auto search_result = search_server.FindTopDocuments("funny"s, DocumentStatus::ACTUAL);
        for (auto tmp : search_result) {
            PrintDocument(tmp);
            cout << "all stop-list" << endl;
        }
    }
    {
        const auto [words, status] = search_server.MatchDocument("pet"sv, 1);
        cout << words.size() << " words for document 1"s << endl;
    }
}

    





