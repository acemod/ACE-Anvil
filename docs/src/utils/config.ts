import fs from 'node:fs'
import yaml from 'js-yaml'

export interface SiteConfig {
  name: string
  site: string
  base: string
  description: string
  keywords: string
  themeColor: string
}

export interface AceConfig {
  githubUrl: string
  version: AceVersion
}

export interface AceVersion {
  major: number
  minor: number
  patch: number
  build: number
}

export interface AppBlogConfig {
  postsPerPage: number
  list: {
    pathname: string
  }
  post: {
    permalink: string
  }
  category: {
    pathname: string
  }
  tag: {
    pathname: string
  }
}

export const config = yaml.load(fs.readFileSync('src/config.yaml', 'utf8')) as {
  site: SiteConfig
  ace: AceConfig
}
