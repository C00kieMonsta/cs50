#!/usr/bin/env php

<?php


require("dictionary.php");
define("LENGTH", 45);
define(DICTIONARY, "/home/cs50/pset5/dictionaries/large");


    // check for correct number of args
    if ($argc != 2 && $argc != 3)
    {
        printf("Usage: speller [dictionary] text\n");
        return 1;
    }


    // benchmarks
    $time_load = 0.0;
    $time_check = 0.0;
    $time_size = 0.0;
    $time_unload = 0.0;

    // determine dictionary to use
    // if argc == 3, than we use the dictonary that we've put in argument
    // else we use DICTIONARY which is the default dectionary if argc == 2
    $dictionary = ($argc == 3) ? $argv[1] : DICTIONARY;

    // load dictionary
    $before = microtime(true);
    $loaded = load(dictionary);
    $after = microtime(true);


    // abort if dictionary not loaded
    if (!$loaded)
    {
        printf("Could not load %s.\n", $dictionary);
        return 1;
    }

    // calculate time to load dictionary
    $time_load = $after - $before;

    // try to open text
    $text = ($argc == 3) ? $argv[2] : $argv[1];
    $fp = fopen($text, "r");
    if ($fp == NULL)
    {
        printf("Could not open $text.\n");
        return 1;
    }

    // prepare to report misspellings
    printf("\nMISSPELLED WORDS\n\n");

    // prepare to spell-check
    $index = 0;
    $misspellings = 0;
    $words = 0;
    $word[LENGTH+1];

    // spell-check each word in text
    for ($c = fgetc($fp); $c !== false; $c = fgetc($fp))
    {
        // allow only alphabetical characters and apostrophes
        if (preg_match("/[a-zA-Z]/", $c) || ($c == "'" && $index > 0))
        {
            // append character to word
            $word .= $c;
            $index++;

            // ignore alphabetical strings too long to be words
            if ($index > LENGTH)
            {
                // consume remainder of alphabetical string
                while (($c = fgetc($fp)) !== false && preg_match("/[a-zA-Z]/", $c));

                // prepare for new word
                $index = 0;
                $word = "";
            }
        }

        // ignore words with numbers (like MS Word can)
        else if (ctype_digit($c))
        {
            // consume remainder of alphanumeric string
            while (($c = fgetc($fp)) != false && preg_match("/[a-zA-z0-9]/", $c));

            // prepare for new word
            $index = 0;
            $word = "";
        }

        // we must have found a whole word
        else if ($index > 0)
        {
            // update counter
            $words++;

            // check word's spelling
            $before = microtime(true);
            $misspelled = !check($word);
            $after = microtime(true);

            // update benchmark
            $time_check += $after - $before;

            // print word if misspelled
            if ($misspelled)
            {
                print("$word\n");
                $misspellings++;
            }

            // prepare for next word
            $index = 0;
            $word = "";
        }
    }

    // close text
    fclose($fp);
  

    // determine dictionary's size
    $before = microtime(true);
    $n = size();
    $after = microtime(true);

    // calculate time to determine dictionary's size
    $time_size = $after - $before;


    // calculate time to unload dictionary
    $time_unload = $after - $before;

    // report benchmarks
    printf("\nWORDS MISSPELLED:    $misspellings \n");
    printf("WORDS IN DICTIONARY: $n \n");
    printf("WORDS IN TEXT:        $words\n");
    printf("TIME IN load:         $time_load\n");
    printf("TIME IN check:        $time_check\n");
    printf("TIME IN size:         $time_size\n");
    printf("TIME IN unload:       $time_unload\n");
    printf("TIME IN TOTAL:        $time_load + $time_check + $time_size + $time_unload\n\n");

    // that's all folks
    return 0;
?>
