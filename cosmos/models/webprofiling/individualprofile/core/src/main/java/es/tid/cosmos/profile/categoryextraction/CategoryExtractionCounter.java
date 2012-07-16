package es.tid.cosmos.profile.categoryextraction;

/*
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol
 */
public enum CategoryExtractionCounter {
    UNPROCESSED_VISITS,
    WRONG_FILTERING_FIELDS,
    KNOWN_VISITS,
    IRRELEVANT_VISITS,
    UNKNOWN_VISITS
}