/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.base.mapreduce;

import java.io.IOException;

/**
 *
 * @author ximo
 */
abstract class ExceptionedThread extends Thread {
        private Exception exception;

        protected void setException(Exception e) {
            this.exception = e;
        }

        public void throwErrors() throws IOException, InterruptedException,
                                         ClassNotFoundException {
            if (this.exception == null) {
                return;
            }

            if (this.exception instanceof RuntimeException) {
                throw (RuntimeException)this.exception;
            } else if (this.exception instanceof IOException) {
                throw (IOException)this.exception;
            } else if (this.exception instanceof InterruptedException) {
                throw (InterruptedException)this.exception;
            } else if (this.exception instanceof ClassNotFoundException) {
                throw (ClassNotFoundException)this.exception;
            } else {
                throw new RuntimeException("Unexpected exception thrown",
                                           this.exception);
            }
        }
    }
