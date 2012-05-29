package es.tid.cosmos.platform.injection.server;

import org.apache.commons.cli.*;

/**
 * @author sortega
 */
public class ServerCommandLine {
    private static final String CONFIG_FILE = "c";

    private final GnuParser parser;
    private final Options options;
    private CommandLine commandLine;

    public ServerCommandLine() {
        this.parser = new GnuParser();
        this.options = new Options().addOption(CONFIG_FILE, "config", true,
                                               "Configuration file");
    }

    public void parse(String[] args) throws ParseException {
        this.commandLine = this.parser.parse(this.options, args.clone());
    }

    public void printUsage() {
        new HelpFormatter().printHelp("injection-server", this.options);
    }

    public boolean hasConfigFile() {
        return this.commandLine.hasOption(CONFIG_FILE);
    }

    public String getConfigFile() {
        return this.commandLine.getOptionValue(CONFIG_FILE);
    }
}
