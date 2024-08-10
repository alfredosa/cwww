#include "routes.h"
#include <dirent.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

BlogPost *blog_posts = NULL;
int post_count = 0;

char *extract_metadata(const char *content, const char *key) {
  char search_key[256];
  snprintf(search_key, sizeof(search_key), "%s:", key);
  char *start = strstr(content, search_key);
  if (start) {
    start += strlen(search_key);
    char *end = strchr(start, '\n');
    if (end) {
      int length = end - start;
      char *value = malloc(length + 1);
      strncpy(value, start, length);
      value[length] = '\0';
      return value;
    }
  }
  return NULL;
}

char *format_nice_date(const char *iso_date) {
  struct tm tm;
  char nice_date[64];
  memset(&tm, 0, sizeof(struct tm));
  strptime(iso_date, "%Y-%m-%dT%H:%M", &tm);
  strftime(nice_date, sizeof(nice_date), "%B %d, %Y", &tm);
  return strdup(nice_date);
}

char *create_url_friendly_name(const char *filename) {
  char *name = strdup(filename);
  char *dot = strrchr(name, '.');
  if (dot)
    *dot = '\0'; // Remove file extension

  // Replace spaces with hyphens and convert to lowercase
  for (char *p = name; *p; p++) {
    if (*p == ' ')
      *p = '-';
    else
      *p = tolower(*p);
  }

  return name;
}

char **extract_tags(const char *content, int *tag_count) {
  char *tags_start = strstr(content, "tags:");
  if (!tags_start)
    return NULL;

  char **tags = NULL;
  *tag_count = 0;

  // Move to the start of the next line after "tags:"
  char *line_start = strchr(tags_start, '\n');
  if (!line_start)
    return NULL;
  line_start++; // Move past the newline

  while (*line_start) {
    // Check if the line starts with a space, then a dash, then a space
    if (line_start[0] == ' ' && line_start[1] == '-' && line_start[2] == ' ') {
      char *tag = line_start + 3; // Move past " - "

      // Find the end of the tag (newline or end of string)
      char *tag_end = strchr(tag, '\n');
      if (!tag_end)
        tag_end = tag + strlen(tag);

      // Allocate memory for the new tag
      int tag_length = tag_end - tag;
      char *new_tag = malloc(tag_length + 1);
      strncpy(new_tag, tag, tag_length);
      new_tag[tag_length] = '\0';

      // Trim any trailing whitespace
      char *end = new_tag + tag_length - 1;
      while (end > new_tag && isspace((unsigned char)*end))
        end--;
      end[1] = '\0';

      // Add the tag to the array
      tags = realloc(tags, (*tag_count + 1) * sizeof(char *));
      tags[*tag_count] = new_tag;
      (*tag_count)++;

      // Move to the next line
      line_start = tag_end + 1;
    } else {
      // If we encounter a line that doesn't start with " - ", we've reached the
      // end of the tags section
      break;
    }
  }

  return tags;
}

void get_all_blog_posts_and_set_routes(HTTPServer *server) {
  printf("Entering get_all_blog_posts_and_set_routes\n");
  const char *path = "/home/alfie/.config/fifisv/hub/Home/StudyHall/Blog";
  printf("Blog path: %s\n", path);
  DIR *dir;
  struct dirent *ent;
  printf("About to open directory\n");
  dir = opendir(path);
  if (dir == NULL) {
    perror("Unable to open directory");
    return;
  }
  printf("Directory opened successfully\n");

  while ((ent = readdir(dir)) != NULL) {
    printf("Reading directory entry\n");
    if (ent->d_type == DT_REG) {
      if (strcmp(ent->d_name, "Blog.md") == 0) {
        printf("Skipping Blog.md file\n");
        continue;
      }

      char full_path[512];
      snprintf(full_path, sizeof(full_path), "%s/%s", path, ent->d_name);
      printf("Found file: %s\n", full_path);

      char *content = read_file(full_path);
      printf("File content read\n");

      if (content) {
        BlogPost post;
        post.filename = strdup(ent->d_name);

        post.url_friendly_name = create_url_friendly_name(ent->d_name);

        post.created_date = extract_metadata(content, "Created");
        post.published_date = extract_metadata(content, "Published");
        post.nice_date = format_nice_date(
            post.published_date ? post.published_date : post.created_date);

        post.tags = extract_tags(content, &post.tag_count);

        blog_posts = realloc(blog_posts, (post_count + 1) * sizeof(BlogPost));
        if (blog_posts == NULL) {
          printf("Failed to reallocate memory for blog_posts\n");
          exit(1);
        }
        blog_posts[post_count++] = post;

        char route[256];
        snprintf(route, sizeof(route), "/blog/%s", post.url_friendly_name);
        printf("Adding route: %s\n", route);
        add_route(server, GET, route, blog_post);

        free(content);
        printf("File content freed\n");
      } else {
        printf("Failed to read file content\n");
      }
    }
    printf("Finished processing directory entry\n");
  }

  closedir(dir);
  printf("Directory closed\n");
  printf("Exiting get_all_blog_posts_and_set_routes\n");
}

char *generate_blog_list_html() {
  char *html = strdup("<ul class=\"blog-posts\">\n");
  for (int i = 0; i < post_count; i++) {
    char li[2048]; // Increased buffer size to accommodate tags
    const char *date_to_use = blog_posts[i].published_date
                                  ? blog_posts[i].published_date
                                  : blog_posts[i].created_date;

    char tags_html[1024] = "";
    if (blog_posts[i].tag_count > 0) {
      strcat(tags_html, " <span class=\"tags\">[");
      for (int j = 0; j < blog_posts[i].tag_count; j++) {
        if (j > 0)
          strcat(tags_html, ", ");
        char tag_span[128];
        snprintf(tag_span, sizeof(tag_span), "<span class=\"tag\">%s</span>",
                 blog_posts[i].tags[j]);
        strcat(tags_html, tag_span);
      }
      strcat(tags_html, "]</span>");
    }

    snprintf(li, sizeof(li),
             "  <li>\n"
             "    <span>\n"
             "      <i><time datetime=\"%s\">%s</time></i>\n"
             "    </span>\n"
             "    <a href=\"/blog/%s\">%s</a>%s\n"
             "  </li>\n",
             date_to_use, blog_posts[i].nice_date,
             blog_posts[i].url_friendly_name, blog_posts[i].filename,
             tags_html);

    html = realloc(html, strlen(html) + strlen(li) + 1);
    strcat(html, li);
  }
  char *closing = "</ul>\n";
  html = realloc(html, strlen(html) + strlen(closing) + 1);
  strcat(html, closing);
  return html;
}

void free_blog_post(BlogPost *post) {
  free(post->filename);
  free(post->url_friendly_name);
  free(post->created_date);
  free(post->published_date);
  free(post->nice_date);
  for (int i = 0; i < post->tag_count; i++) {
    free(post->tags[i]);
  }
  free(post->tags);
}