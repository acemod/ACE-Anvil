import antfu from '@antfu/eslint-config'

export default antfu(
  {
    astro: true,
    rules: {
      curly: ['error', 'all'],
    },
  },
)
