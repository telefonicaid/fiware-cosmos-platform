package es.tid.ps.flume;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import org.apache.log4j.Logger;

import com.cloudera.flume.conf.Context;
import com.cloudera.flume.conf.SourceFactory.SourceBuilder;
import com.cloudera.flume.core.Event;
import com.cloudera.flume.core.EventImpl;
import com.cloudera.flume.core.EventSource;
import com.cloudera.util.Pair;
import com.google.common.base.Preconditions;

public class BlockFileSource extends EventSource.Base {
	static Logger LOG = Logger.getLogger(BlockFileSource.class);
	static int bufferSize = 1024 * 1024 * 4;
	String fname;
	FileInputStream ins;
	long len;
	long cur;
	int bufSize;

	public BlockFileSource(String string, int size) {
		fname = string;
		bufSize = size;
	}

	public Event next() throws IOException {
		Preconditions.checkState(ins != null,
				"Need to open source before reading from it");
		byte[] s = new byte[bufSize];
		int readed = ins.read(s);
		if (readed == -1) {
			return null;
		}
		byte[] ss;
		if (bufSize == readed) {
			ss = s;
		} else {
			ss = new byte[readed];
			for (int ii = 0; ii < readed; ii++) {
				ss[ii] = s[ii];
			}
		}
		Event e = new EventImpl(ss);
		updateEventProcessingStats(e);
		return e;
	}

	@Override
	public void close() throws IOException {
		ins.close();
		LOG.info("File " + fname + " closed");
	}

	@Override
	public void open() throws IOException {
		this.ins = new FileInputStream(fname);
		File file = new File(fname);
		this.len = file.length();
		this.cur = 0;
		LOG.info("File " + fname + " opened, size=" + this.len);
	}

	public static SourceBuilder builder() {
		return new SourceBuilder() {
			@Override
			public EventSource build(String... argv) {
				Preconditions.checkArgument(argv.length >= 1
						&& argv.length <= 2,
						"usage: BlockFileSource(file, bufferSize) ");
				int bufSize = bufferSize;
				if (argv.length == 2) {
					bufSize = Integer.valueOf(argv[1]);
					if (bufSize <= 0) {
						bufSize = bufferSize;
					}
				}
				return new BlockFileSource(argv[0], bufSize);
			}

			@Override
			public EventSource build(Context ctx, String... argv) {
				return build(argv);
			}
		};
	}

	public static List<Pair<String, SourceBuilder>> getSourceBuilders() {
		List<Pair<String, SourceBuilder>> builders = new ArrayList<Pair<String, SourceBuilder>>();
		builders.add(new Pair<String, SourceBuilder>("blockFileSource",
				builder()));
		return builders;
	}
}