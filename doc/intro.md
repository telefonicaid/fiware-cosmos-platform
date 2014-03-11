# Cosmos Introduction

## Motivation

Big Data consists of datasets that grow so large that they become awkward to work with
using on-hand database management tools. The data is usually very large, needs to be updated
or stored at very high rates and may also have an inconsistent quality. In addition to this,
processing such large volumes must also be accomplished from a non-traditional approach, in
order to extract the full potential of that data.

For Telefónica, it is clear that we need to take advantage from our massive amount of data
that our products, services and users generate. Cosmos enables us not only to have that data
accessible in a flexible manner, but also to process it in order to gain a deeper knowledge
of our business.

The approach followed to address the building of Cosmos is twofold: on the one hand it is
entitled to allow **exploratory analytics** in a flexible way, and on the other it is
conceived as a way of **enabling new products and services**. The former puts requirements
on the data (accessibility, variety, volume and freshness) and the processing capabilities
(computation time, distributed processing languages or tools and visualization). The latter
emphasizes the need to have an industrialized big data analytical platform that enables its
knowledge consumption by other products and services.

## Philosophy

Cosmos follows the same philosophy as Amazon's Elastic MapReduce: computation clusters are
assumed to be short-lived (they are created before a specific computation needs to happen and
terminated once the computation is done) and persistent data is stored in a different layer.

We believe that creating and destroying clusters on a frequent basis provides several benefits
over having a single, long-running Hadoop cluster which is shared through a fair Hadoop
scheduler.

Newly created clusters always have a clean, predictable and stable state. Long-running clusters
need state management and are prone to users mistakenly or maliciously changing the state of the
cluster which affects all other users. Having a predictable state when starting a computation
reduces the delivery time for products and services that run on Cosmos since they don't need
to protect themselves against complicated states that might arise on long-running clusters.
This reduces the overall time-to-market of Big Data based products.

Having separate clusters also makes it easier to monitor the infrastructure usage on a per-user
basis, which leads to better utilization of the resources.
