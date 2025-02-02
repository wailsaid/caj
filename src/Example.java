package src;

import org.apache.commons.lang3.StringUtils;
import org.apache.commons.lang3.text.WordUtils;

public class Example {
    public static void main(String[] args) {
        String text = "hello world";
        
        // Using Commons Lang3 utilities
        System.out.println("Original text: " + text);
        System.out.println("Capitalized: " + StringUtils.capitalize(text));
        System.out.println("Is blank? " + StringUtils.isBlank(text));
        System.out.println("Reversed: " + StringUtils.reverse(text));
        
        String multiline = "this is a long\ntext that spans\nmultiple lines";
        System.out.println("\nWrapped text:");
        System.out.println(WordUtils.wrap(multiline, 20));
    }
} 