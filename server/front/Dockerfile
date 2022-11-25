FROM node:16-alpine3.14

WORKDIR /app

COPY . /app
RUN yarn generate

CMD ["yarn", "start"]

EXPOSE 3000
ENV HOST 0.0.0.0