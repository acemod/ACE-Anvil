import { defineConfig } from 'astro/config'
import starlight from '@astrojs/starlight'
import tailwind from '@astrojs/tailwind'

import { config } from './src/utils/config'

// https://astro.build/config
export default defineConfig({
  site: config.site.url,
  integrations: [
    starlight({
      title: config.site.name,
      logo: {
        dark: './src/assets/logo-white.webp',
        light: './src/assets/logo-black.webp',
        replacesTitle: true,
      },
      customCss: [
        // Path to your Tailwind base styles:
        './src/styles/tailwind.css',
      ],
      social: {
        'discord': config.ace.discordUrl,
        'github': config.ace.githubUrl,
        'x.com': 'https://twitter.com/ACE3Mod',
        'facebook': 'https://facebook.com/ACE3Mod',
        'youtube': 'https://youtube.com/c/ACE3Mod',
      },
      sidebar: [
        {
          label: 'Overview',
          items: [
            { label: 'General Information', link: '/general/' },
            { label: 'Frequently Asked Questions', link: '/faq/' },
            { label: 'Team', link: '/team/' },
            { label: 'Branding', link: '/branding/' },
          ],
        },
        //{
        //  label: 'News',
        //  collapsed: true,
        //  autogenerate: { directory: 'news' },
        //},
        {
          label: 'Guides',
          autogenerate: { directory: 'guides' },
        },
        {
          label: 'Components',
          autogenerate: { directory: 'components' },
        },
      ],
      locales: {
        root: {
          label: 'English',
          lang: 'en',
        },
      },
      editLink: {
        baseUrl: config.ace.githubUrl + '/edit/master/docs/',
      },
      lastUpdated: true,
    }),
    tailwind({
      // Disable the default base styles:
      applyBaseStyles: false,
    }),
  ],
})
