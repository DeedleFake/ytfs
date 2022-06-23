ytfs
====

ytfs is an experimental wrapper around [nvi][nvi] (Or possibly not, as that might complicate things.) to provide a filesystem interface. It doesn't work yet. The goal is that it will provide a ctl file that can be used to fetch information about a given file by creating a directory corresponding to it, allowing the title, description, and other pieces of metadata to be acquired, as well as allowing the actual raw data itself to be streamed.

Here's the general idea, written up before actual implementation:

```rc
% echo -n 'fetch dP1xVpMPn8M' > /mnt/yt/ctl
% cd /mnt/yt/dP1xVpMPn8M
% lc
title		description	tags		streams/
% cat title <{echo}
A Tour of the Acme Editor
```

[nvi]: https://git.sr.ht/~ft/nvi