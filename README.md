# Documentation
You can find the information provided in this readme and the complete documentation here: [https://vrgrouprwth.github.io/insite/](https://vrgrouprwth.github.io/insite/)

# Insite - A Pipeline Enabling In-Transit Access for NEST, TVB and Arbor.
This project/research has received funding from the European Union’s Horizon 2020 Framework Programme for Research and Innovation under the
Specific Grant Agreement No. 945539 (Human Brain Project SGA3) and Specific Grant Agreement No. 785907 (Human Brain Project SGA2).

## Goal 
Insite provides a middleware that enables users to acquire data from neural simulators via the in-transit paradigm.
In-transit approaches allow users to access data from a running simulation while the simulation is still going on.
In the traditional approach data from simulations is written to disk first, and can only be accessed after the simulation has finished.
However, this has two main constraints:
  1. Data can only be further processed after the whole simulation has finished.
  2. Disk speed can be a bottleneck when simulating, as data has to be written out.
  3. Data must be completely stored on the machine, leading to large files.

Using **Insite** allows users to develop data consumer, such as visualizations and analysis tools that allow **early insight** into the data **without storing data** with virtually **zero dependencies**.

**Insite** was specifically designed to be **ease-to-integrate** and **easy-to-use** to allow a wide range of users to take advantage of in-transit approaches in the context of brain simulatiion.
Insite uses off-the-shelf dataformats and protocols to make integration as easy as possible.
Data can be queried via an HTTP REST API from Insite's access node, which represents a single point of contact for the user.
