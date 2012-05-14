package es.tid.cosmos.platform.injection.server;

/**
 * InjectionUnathorizedPathException
 * see COPYRIGHT or LICENSE for terms of use
 *
 * @author logc
 * @since 11/05/12
 */
public class InjectionUnathorizedPathException extends RuntimeException {
    InjectionUnathorizedPathException(String message) {
        super(message);
    }
}
