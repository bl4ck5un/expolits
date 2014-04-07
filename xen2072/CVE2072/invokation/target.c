// xc.vcpu_setaffinity(domid, vcpu, cpumap)
static PyObject *pyxc_vcpu_setaffinity(XcObject *self,
                                       PyObject *args,
                                       PyObject *kwds)
{
    uint32_t dom;
    int vcpu = 0, i;
    xc_cpumap_t cpumap;
    PyObject *cpulist = NULL;
    int nr_cpus;

    static char *kwd_list[] = { "domid", "vcpu", "cpumap", NULL };

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|iO", kwd_list, 
                                      &dom, &vcpu, &cpulist) )
        return NULL;

    // nr_cpus = xc_get_max_cpus(self->xc_handle);
    // if ( nr_cpus == 0 )
    //     return pyxc_error_to_exception(self->xc_handle);

    cpumap = xc_cpumap_alloc(self->xc_handle);
    if(cpumap == NULL)
        return pyxc_error_to_exception(self->xc_handle);

    if ( (cpulist != NULL) && PyList_Check(cpulist) )
    {
        for ( i = 0; i < PyList_Size(cpulist); i++ ) 
        {
            long cpu = PyInt_AsLong(PyList_GetItem(cpulist, i)); // get i item from cpulist
            // if ( cpu < 0 || cpu >= nr_cpus )
            // {
            //     free(cpumap);
            //     errno = EINVAL;
            //     PyErr_SetFromErrno(xc_error_obj);
            //     return NULL;
            // }
            cpumap[cpu / 8] |= 1 << (cpu % 8);
        }
    }
  
    if ( xc_vcpu_setaffinity(self->xc_handle, dom, vcpu, cpumap) != 0 )
    {
        free(cpumap);
        return pyxc_error_to_exception(self->xc_handle);
    }
    Py_INCREF(zero);
    free(cpumap); 
    return zero;
}