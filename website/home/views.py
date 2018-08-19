from django.shortcuts import render

# views

def index(request):
    """shows homepage upon request, along with any account information
    
    Arguments:
        request {string} -- managed by django
    """
    
    return render(request, 'home.html', content_type='text/html')
