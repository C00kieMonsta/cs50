#!/usr/bin/env php

<?php

    $size = 0;
    
    $table = [];
    
    function check($word)
    {
        global $table;
        
        if(isset($table[strtolower($word)]))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    function load($dictionnary)
    {
        global $size;
        global $table;
        
        //PHP has a function called file whose purpose in life is to open a 
        // file and read in all of its lines into an array and hand them back to
        
        foreach(file($dictionnary) as $word)
        {
            $table[chop($word)] = true;
            $size++; 
        }
    }
    
    function size()
    {
        global $size;
        return $size;
    }



?>
