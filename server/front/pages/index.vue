<template>
<div>
  <h1>Koken電光掲示板</h1>
  <p v-html="str"></p>
  <input type="text" v-model="input">
  <button @click="send">送信</button>
</div>
</template>

<script lang="ts">
import Vue from 'vue'
import axios from 'axios'

export default Vue.extend({
  name: 'IndexPage',
  data(){
    return{
      str: "",
      input: ""
    }
  },
  methods:{
    async send(){
      if(this.input==""){
        alert("Input something!");
      }
      const data = (await axios.post(`http://apilcd.home.k1h.dev/json/${this.input}`));
      if(data.status==200){
        console.log("OK")
        this.update();
      }else{
        console.log("Error")
      }
    },
    async update(){
      this.str="";
    const data = (await axios.get("http://apilcd.home.k1h.dev/json")).data;
    let lcdOutput:number[] = new Array();
    let i:number,j:number;
    for(j=0;j<8;j++){
      let cal = 1 << j;
      for(i=0;i<data.data.length;i++){
        if(data.data[i]&cal){
          this.str+="●";
        }else{
          this.str+="○";
        }
      }
      this.str+="<br>";
    }
    }
  },
  async mounted() {
   this.update();
  }
})
</script>

<style>
body{
font-family:monospace;
}
</style>