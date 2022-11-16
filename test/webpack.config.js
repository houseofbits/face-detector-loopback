const path = require('path');

module.exports = {
    entry: {
        'main': './src/main.js',
    },
    output: {
        filename: '[name].js',
        publicPath: '/',
        path: path.resolve(__dirname, 'dist/'),
    },
    stats: {
        children: true,
    },
    devServer: {
        index: 'index.html',
        contentBase: path.join(__dirname, 'dist'),
        compress: true,
        host: '0.0.0.0',
        port: 9001,
        open: false,
        historyApiFallback: {
            rewrites: [
                {from: /^\//, to: '/'}
            ]
        }
    }
};