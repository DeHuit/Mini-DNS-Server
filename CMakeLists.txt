cmake_minimum_required(VERSION 3.8)
project(mini_dns_etu)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        RR/root.conf
        RR/tld.fr.conf
        RR/univ-tlse3.fr.conf
        RR/wikipedia.fr.conf
        tests/ref/authoritative/ftp.univ-tlse3.fr
        tests/ref/authoritative/www.univ-tlse3.fr
        tests/ref/authoritative/zinconnu.univ-tlse3.fr
        tests/ref/full/ftp.univ-tlse3.fr
        tests/ref/full/media.wikipedia.fr
        tests/ref/full/www.univ-tlse3.fr
        tests/ref/full/www.wikipedia.fr
        tests/ref/full/zinconnu.univ-tlse3.fr
        tests/ref/resolver/webmail.gandi.net
        tests/ref/resolver/www.free.fr
        tests/ref/resolver/www.irit.fr
        tests/ref/resolver/www.radioclassique.fr
        tests/ref/resolver/zinconnu.univ-tlse3.fr
        tests/ref/root/test.root.fr
        tests/ref/root/test.root.net
        tests/ref/tld/test.univ-tlse3.fr
        tests/ref/tld/test.wikipedia.fr
        tests/test_dig_10.txt
        tests/test_dig_100.txt
        tests/test_dig_1000.txt
        tests/test_dig_10000.txt
        dns_client.c
        dns_client3.c
        dns_packet_parsing.c
        dns_packet_parsing.h
        dns_server_rr_files.c
        dns_server_rr_files.h
        dns_server_v0.c
        dns_server_v1.c
        Makefile
        test.c
        test_script.sh
        utils.c
        utils.h
        dns_server_local_v1.c
        dns_server_local_v2.c
        dns_server_v2.c
        dns_client_v2.c)

add_executable(mini_dns_etu ${SOURCE_FILES})