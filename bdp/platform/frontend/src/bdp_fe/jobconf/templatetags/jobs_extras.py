
from django import template
register = template.Library()

@register.filter
def get_type(obj):
    """
    Gets the type of an object from within a template. Useful for checking the
    subcomponents of a complex class.
    """
    return obj.__class__.__name__
