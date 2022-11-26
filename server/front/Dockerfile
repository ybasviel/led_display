FROM node:16-alpine3.14

WORKDIR /app

COPY . /app
RUN yarn install
RUN yarn build

CMD ["yarn", "start"]

EXPOSE 3000
ENV HOST 0.0.0.0
