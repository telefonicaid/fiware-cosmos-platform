package es.tid.ps.profile;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.NavigableMap;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.Get;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.util.Bytes;

import es.tid.ps.profile.data.UserNavigation;
import es.tid.ps.profile.dictionary.Categorization;
import es.tid.ps.profile.dictionary.Dictionary;
import es.tid.ps.profile.dictionary.comscore.CSDictionary;

/**
 * Application entry point.
 *
 * @author sortega
 */
public class DirectProfileExport {
    private static final String DICTIONARY_DIR =
            "/home/hdfs/direct-profile/comscore";
    private static final Path[] DICTIONARY_FILES = new Path[] {
        new Path(DICTIONARY_DIR, "cs_terms_in_domain.bcp"),
        new Path(DICTIONARY_DIR, "cs_mmxi.bcp.gz"),
        new Path(DICTIONARY_DIR, "patterns_to_categories.txt"),
        new Path(DICTIONARY_DIR, "cat_subcat_map.txt"),
    };
    private static final byte[] CATEGORY_FAMILY = Bytes.toBytes("cat");

    private final String inputFilename;
    private final Dictionary dictionary;
    private int count;
    private int knownCount;
    private int unknownCount;
    private int irrelevantCount;
    private int unprocessedCount;
    private HTable profileTable;


    public DirectProfileExport(String inputFilename) {
        this.inputFilename = inputFilename;
        this.dictionary = new CSDictionary(DICTIONARY_FILES);
    }

    public void export() throws FileNotFoundException, IOException {
        BufferedReader input = new BufferedReader(new FileReader(inputFilename));
        dictionary.init();
        initHBase();

        String line;
        while((line = input.readLine()) != null) {
            if (count % 1000 == 0) {
                System.out.format("%d logs imported\n", count);
            }
            count++;
            UserNavigation un = UserNavigation.parse(line);
            Categorization dictResponse = dictionary.categorize(un.getUrl());

            switch (dictResponse.getResult()) {
                case KNOWN_URL:
                    knownCount++;
                    updateProfile(un, dictResponse.getCategories());
                    break;

                case IRRELEVANT_URL:
                    irrelevantCount++;
                    break;

                case UNKNOWN_URL:
                    unknownCount++;
                    break;

                case GENERIC_FAILURE:
                    unprocessedCount++;
                    break;

                default:
                    throw new IllegalStateException("Invalid result");
            }
        }
    }

    public static void main(String[] args) throws IOException {
        if (args.length != 1) {
            System.err.println("Usage: <command> input_file");
            System.exit(1);
        }

        try {
            new DirectProfileExport(args[0]).export();
        } catch (FileNotFoundException ex) {
            System.err.println("File not found: " + args[0]);
            System.exit(1);
        }

        System.exit(0);
    }

    private void initHBase() throws IOException {
        Configuration config = HBaseConfiguration.create();
        config.addResource(getClass().getResource("config.xml"));
        this.profileTable = new HTable(config, "directprofile");
    }

    private void updateProfile(UserNavigation un, String[] categories)
            throws IOException {
        final byte[] rowId = Bytes.toBytes(un.getVisitorId() + "@" + un.getDate());

        Result getRequest = this.profileTable.get(new Get(rowId));
        NavigableMap<byte[], NavigableMap<byte[], byte[]>> currentRow =
                getRequest.getNoVersionMap();


        Put insertRequest = new Put(rowId);
        for (String category : categories) {
            final byte[] categoryName = Bytes.toBytes(category);
            final NavigableMap<byte[], byte[]> count = currentRow.get(categoryName);
            long oldCount;
            if (count != null) {
                oldCount = Bytes.toLong(count.firstKey());
            } else {
                oldCount = 0;
            }

            insertRequest.add(CATEGORY_FAMILY, categoryName,
                    Bytes.toBytes(oldCount + 1));
        }
    }
}
