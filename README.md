# This is my webside :)

Bare git, pure C, No Javascript

## Instructions

### Install dependencies

```bash
sudo apt install build-essential cmake git certbot python3-certbot-nginx
```

### Build the server

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Export your domain

```bash
### Replace with your domain
export YOUR_DOMAIN=blog.alfie.news
```

### Run the server / Enable systemd service

```bash
cp cwww.service /etc/systemd/system/cwww.service
systemctl daemon-reload
systemctl enable cwww.service
sudo systemctl start cwww.service
```

## ngnix config && certbot (TLS)

```bash
cp ngnix.conf /etc/nginx/sites-available/$(YOUR_DOMAIN)
ln -s /etc/nginx/sites-available/$(YOUR_DOMAIN) /etc/nginx/sites-enabled/$(YOUR_DOMAIN)

ngnix -t
```

#### cloudflare

Get IP from host, then add an A record to cloudflare with the IP

#### Enable

```bash
systemctl reload nginx

certbot --nginx -d $(YOUR_DOMAIN)
```

#### RENEWAL of certificates:

```bash
certbot renew --dry-run
certbot renew
```

#### Check

If it all goes well, you should be able to access the site via https://$(YOUR_DOMAIN)
