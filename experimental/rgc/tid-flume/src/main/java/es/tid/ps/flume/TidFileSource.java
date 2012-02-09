package es.tid.ps.flume;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.cloudera.flume.conf.Context;
import com.cloudera.flume.conf.SourceFactory.SourceBuilder;
import com.cloudera.flume.core.Event;
import com.cloudera.flume.core.EventImpl;
import com.cloudera.flume.core.EventSource;
import com.cloudera.util.Pair;
import com.google.common.base.Preconditions;

public class TidFileSource extends EventSource.Base {

	//static final Logger LOG = LoggerFactory.getLogger(TidFileSource.class);

	static int bufferSize = 1024 * 1024 * 4;
	private FileInputStream ins;
	private final String path;

	public TidFileSource(String path) {
		this.path = path;
	}

	@Override
	public void close() throws IOException {
		ins.close();
//		LOG.info("File " + path + " closed");
	}

	@Override
	public void open() throws IOException {
		this.ins = new FileInputStream(path);
//		LOG.info("File " + path + " opened");
	}

	public Event next() throws IOException {
		Preconditions.checkState(ins != null,
				"Need to open source before reading from it");
		byte[] s = new byte[bufferSize];
		int readed = ins.read(s);
		if (readed == -1) {
			return null;
		}
		byte[] ss;
		if (bufferSize == readed) {
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

	public static SourceBuilder builder() {
		// construct a new parameterized source
		return new SourceBuilder() {
			@Override
			public EventSource build(Context ctx, String... argv) {
				Preconditions.checkArgument(argv.length == 1,
						"usage: TtidFileSource path");

				return new TidFileSource(argv[0]);
			}
		};
	}

	public static List<Pair<String, SourceBuilder>> getSourceBuilders() {
		List<Pair<String, SourceBuilder>> builders = new ArrayList<Pair<String, SourceBuilder>>();
		builders.add(new Pair<String, SourceBuilder>("tidFileSource", builder()));
		return builders;
	}

}
